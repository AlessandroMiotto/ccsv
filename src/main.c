#include <stdio.h>
#include <stdlib.h>
#include "ccsv.h"
#include "df_stat.h"

int main()
{
    DataFrame *df = readCSV("Admission_Predict.csv");
    int skip[1] = {0};
    
    printDataFrame(df);
    printSummary(df, skip, 1);

    freeDataFrame(df);

    return 0;
}