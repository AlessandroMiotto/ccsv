#include "ccsv.h"

int main()
{
    DataFrame *df = readCSV("Admission_Predict.csv");
    int skip[1] = {0};
    
    Summary* summ = __allocate_summary__(df, skip, 1);
    summary(summ, df, skip, 1);

    printDataFrame(df);
    printSummary(df, skip, 1);
    correlationMatrix(df, summ, 1);
    
    __free_summary__(summ);
    freeDataFrame(df);

    return 0;
}