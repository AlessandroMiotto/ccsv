#include "ccsv.h"

int main()
{
    DataFrame *df = read_csv("Admission_Predict.csv");

    printDataFrame(df, ALL_LINES);

    //printSummary(df, (int []){0, 5}, 2);
    //corr(df);

    freeDataFrame(df);

    return 0;
}