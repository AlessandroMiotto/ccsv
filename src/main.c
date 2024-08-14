#include <stdio.h>
#include <stdlib.h>
#include "ccsv.h"
#include "df_stat.h"

int main()
{
    DataFrame *df = readCSV("Admission_Predict.csv");
    //int skip[3] = {1, 2, 3};
    printf("%f\n", __mean__(df, 1));
    //__build_summary__(df, skip, 3);
    freeDataFrame(df);

    return 0;
}