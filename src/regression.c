#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "mkl.h"

#include "ccsv.h"
#include "regression.h"

#define AT(A, i, j) A[i * df->cols + j]

// build the design matrix with num col + 1
// (the first column has only ones to compute the intercept)
void __design_matrix__(double *X, DataFrame *df, int *cols, int n)
{
    #pragma omp parallel for num_threads(NUM_THREADS) collapse(2)
    for (int i = 0; i < df->rows; i++)  
        for (int j = 1; j < n; j++)
            X[i * n + j] = GET(df, i, cols[j - 1]);

    #pragma omp parallel for num_threads(NUM_THREADS)
    for (int i = 0; i < df->rows; i++)
        X[i * n] = 1.0;
}

// Linear regression of the dataframe using least-square estiamtion
// Solve XᵀX β = Xᵀy
void linearRegression(DataFrame *df, LinearRegression lr)
{
    int m = df->rows;
    int n = lr.x_cols_num + 1;
    double* X = (double *)mkl_malloc(sizeof(double) * m * n, 64);
    double* XtX = (double *)mkl_malloc(sizeof(double) * n * n, 64);

    for (int i = 0; i < n; i++)
        lr.beta[i] = 0.0; 

    __design_matrix__(X, df, lr.x_cols, n);

    cblas_dgemm(CblasRowMajor, CblasTrans, CblasNoTrans, n, n, m, 1.0, X, n, X, n, 0.0, XtX, n);

    int* ipiv = (int *)mkl_malloc(sizeof(int) * n, 64);
    LAPACKE_dsytrf(LAPACK_ROW_MAJOR, 'U', n, XtX, n, ipiv);
    LAPACKE_dsytri(LAPACK_ROW_MAJOR, 'U', n, XtX, n, ipiv);
    mkl_free(ipiv);

    double* y = (double *)mkl_malloc(sizeof(double) * m, 64);
    double* Y = (double *)mkl_malloc(sizeof(double) * n, 64);
    
    #pragma omp parallel for num_threads(NUM_THREADS)
    for (int i = 0; i < m; i++)
        y[i] = GET(df, i, lr.y_col);

    #pragma omp parallel for num_threads(NUM_THREADS)
    for (int i = 0; i < n; i++)
        Y[i] = 0.0; 

    #pragma omp parallel for num_threads(NUM_THREADS) collapse(2)
    for (int i = 0; i < n; i++)
        for (int j = i; j < n; j++)
            XtX[j * n + i] = XtX[i * n + j]; 

    cblas_dgemv(CblasRowMajor, CblasTrans, m, n, 1.0, X, n, y, 1, 0.0, Y, 1);
    mkl_free(y);

    cblas_dgemv(CblasRowMajor, CblasNoTrans, n, n, 1.0, XtX, n, Y, 1, 0.0, lr.beta, 1);

    mkl_free(Y);
    mkl_free(X);
    mkl_free(XtX);
    
    if (lr.print == true)
    {
        __print_line__(1, "┌", "─", "┐");
        printf("| %*.*s |\n", 2*CELL_SIZE - 3, 2*CELL_SIZE - 3, "Coefficients");
        __print_line__(1, "├", "┬", "┤");
        printf("| intercept | %+.2e |\n", lr.beta[0]);

        for (int i = 1; i <= lr.x_cols_num; i++)
        {
            __print_line__(1, "├", "┼", "┤");
            printf("| %9.9s | %+.2e |\n", df->label[lr.x_cols[i-1]].nameCol, lr.beta[i]);
        }
        __print_line__(1, "└", "┴", "┘");
    }
}