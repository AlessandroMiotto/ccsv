#include <stdio.h>
#include <stdlib.h>
#include "ccsv.h"

int main()
{
    DataFrame *df = readCSV("Admission_Predict.csv");

    printDataFrame(df);
    
    freeDataFrame(df);

    return 0;
}