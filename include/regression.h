#ifndef H_REGRESSION
#define H_REGRESSION

#include <stdbool.h>

#include "ccsv.h"

typedef struct
{
    double *y;
    int y_col;
    int *x_cols;
    int x_cols_num;
    double *beta;
    bool print;
} LinearRegression;


void __design_matrix__(double *X, DataFrame *df, int *cols, int n);
void __transpose__(double *Xt, double *X, int m, int n);

void __matmul__(double *A, double *B, double *C, int m, int n, int p);
void __forward_substitution__(double *L, double *x, double *b, int n);
void __back_substitution__(double *U, double *x, double *b, int n);
void __cholesky__decomposition__(double *A, double *L, int n);
void __linear_solver__(double *A, double *x, double *b, int n);
void linearRegression(DataFrame *df, LinearRegression lm);

#endif