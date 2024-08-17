#include "ccsv.h"
#include "regression.h"

int main()
{
    DataFrame *df = read_csv("Admission_Predict.csv");
    printDataFrame(df, 10);

    double beta[8];

    LinearRegression model = {
        .y_col = 8,
        .beta = beta,
        .x_cols = (int []){1,2,3,4,5,6,7},
        .x_cols_num = 7,
        .print = false
    };

    linearRegression(df, model);

    return 0;
}