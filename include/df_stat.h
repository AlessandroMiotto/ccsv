#ifndef H_DFSTAT
#define H_DFSTAT

#include "ccsv.h"

typedef struct
{
    double* summary_min;    // min value
    double* summary_Q1;     // first quartile
    double* summary_med;    // median
    double* summary_mean;   // mean
    double* summary_Q3;     // third quartile
    double* summary_max;    // max value
} Summary;

Summary* __allocate_summary__(DataFrame* df, int* cols_skip, const int num_col_skip);
void __free_summary__(Summary* summ);

double __max__(DataFrame* df, int col);
double __min__(DataFrame* df, int col);
double __mean__(DataFrame* df, int col);


#endif