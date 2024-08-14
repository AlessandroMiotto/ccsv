#include <stdio.h>
#include <stdlib.h>
#include "ccsv.h"

int main()
{
    DataFrame *df = readCSV("Admission_Predict.csv");

    printf("Rows: %d\n", df->rows);
    printf("Cols: %d\n", df->cols);

    printf("%f\n", GET(df, 0, 1));
    
    freeDataFrame(df);

    return 0;
}