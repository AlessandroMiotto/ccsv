#include "ccsv.h"
#include "regression.h"

int main()
{

    double A[9] = {4, 12, -16, 12, 37, -43, -16, -43, 98};
    double b[3] = {-4, 5, 128};
    double x[3];

    __linear_solver__(A, x, b, 3);

    for (int i = 0; i < 3; i++)
    {
        printf("%f\n", x[i]);
    }

    /*DataFrame *df = read_csv("Admission_Predict.csv");


    double X[4 * df->rows];
    double Xt[4 * df->rows];
    double XtX[4 * 4];

    __design_matrix__(X, df, (int []){1, 2, 3}, 4);
    __transpose__(Xt, X, 4, df->rows);
    __matmul__(Xt, X, XtX, 4, df->rows, 4);

    freeDataFrame(df);*/

    return 0;
}