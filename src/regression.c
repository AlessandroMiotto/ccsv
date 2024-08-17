#include <stdlib.h>
#include <math.h>
#include <omp.h>

#include "ccsv.h"
#include "regression.h"

#define AT(A, i, j) A[i * df->cols + j]

// build the design matrix with num col + 1
// (the first column has only ones to compute the intercept)
void __design_matrix__(double *X, DataFrame *df, int *cols, int n)
{
    #pragma omp parallel for num_threads(NUM_THREADS)
    for (int i = 0; i < df->rows; i++)
    {
        X[i * n] = 1.0;
        for (int j = 1; j < n; j++)
            X[i * n + j] = GET(df, i, cols[j - 1]);
    }
}

// transpose of matrix X
void __transpose__(double *Xt, double *X, int m, int n)
{
    #pragma omp parallel for num_threads(NUM_THREADS) collapse(2)
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
            Xt[i * n + j] = X[j * m + i];
    }

    /*for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < 10; j++)
            printf("%f\t", Xt[i * n + j]);
        printf("\n");
    }*/
}

// matrix-matrix multiplication
void __matmul__(double *A, double *B, double *C, int m, int n, int p)
{
#pragma omp parallel for num_threads(NUM_THREADS)
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < p; j++)
            for (int k = 0; k < m; k++)
                C[i * p + j] += A[i * n + k] * B[k * p + j];
    }

    /*for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < p; j++)
            printf("%.3e\t", C[i * m + j]);
        printf("\n");
    }*/
}

// Linear system solution Lx=b with L lower matrix
void __forward_substitution__(double *L, double *x, double *b, int n)
{
    for (int i = 0; i < n; i++)
    {
        x[i] = b[i];
        for (int j = 0; j < i; j++)
            x[i] -= L[i * n + j] * x[j];

        x[i] /= L[i * n + i];
    }
}

// Linear system solution Ux=b with U upper matrix
void __back_substitution__(double *U, double *x, double *b, int n)
{
    x[n - 1] = b[n - 1] / U[n * n - 1];
    for (int i = n - 2; i >= 0; i--)
    {
        x[i] = b[i];
        for (int j = i + 1; j < n; j++)
            x[i] -= U[i * n + j] * x[j];

        x[i] /= U[i * n + i];
    }
}

// From a square n x n matrix A find Lower decomposition A=LL^\t
void __cholesky__decomposition__(double *A, double *L, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            float sum = 0;
            for (int k = 0; k < j; k++)
                sum += L[i * n + k] * L[j * n + k];

            if (i == j)
                L[i * n + j] = sqrt(A[i * n + i] - sum);
            else
                L[i * n + j] = (1.0 / L[j * n + j] * (A[i * n + j] - sum));
        }
    }
}

// Linear solver Ax=b for A symmetric n x n matrix
void __linear_solver__(double *A, double *x, double *b, int n)
{
    double *y = (double *)malloc(sizeof(double) * n);
    double *L = (double *)malloc(sizeof(double) * n * n);
    double *Lt = (double *)malloc(sizeof(double) * n * n);

    __cholesky__decomposition__(A, L, n);
    __forward_substitution__(L, y, b, n);
    __transpose__(Lt, L, n, n);
    __back_substitution__(Lt, x, y, n);

    free(y);
    free(L);
    free(Lt);
}
