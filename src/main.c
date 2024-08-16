#include "ccsv.h"

int main()
{
    DataFrame *df = read_csv("Admission_Predict.csv");

    printDataFrame(df);
    
    int skip[1] = {0};
    printSummary(df, skip, 1);
    corr(df);
    
    //printColsName(df);
    
    freeDataFrame(df);

    //#pragma omp parallel for num_threads(NUM_THREADS)

    return 0;
}