#include <stdlib.h>
#include <stdio.h>

#include "ccsv.h"
#include "df_stat.h"

// Build a statistical summary of the columns of the dataframe
Summary* __allocate_summary__(DataFrame* df, int* cols_skip, const int num_col_skip)
{
    Summary* summ = (Summary *)malloc(sizeof(Summary));
    summ->summary_min = (double *)malloc(sizeof(double) * (df->cols - num_col_skip));
    summ->summary_Q1 = (double *)malloc(sizeof(double) * (df->cols - num_col_skip));
    summ->summary_med = (double *)malloc(sizeof(double) * (df->cols - num_col_skip));
    summ->summary_mean = (double *)malloc(sizeof(double) * (df->cols - num_col_skip));
    summ->summary_Q3 = (double *)malloc(sizeof(double) * (df->cols - num_col_skip));
    summ->summary_max = (double *)malloc(sizeof(double) * (df->cols - num_col_skip));
    return summ;
}


// Free memory allocated by __allocate_summary__
void __free_summary__(Summary* summ)
{
    free(summ->summary_min);
    free(summ->summary_Q1);
    free(summ->summary_med);
    free(summ->summary_mean);
    free(summ->summary_Q3);
    free(summ->summary_max);
    free(summ);
}


double __max__(DataFrame* df, int col)
{
    double max = GET(df, 0, col);

    for (int i = 1; i < df->rows; i++)
    {
        if (GET(df, i, col) >= max)
            max = GET(df, i, col);
    }
    
    return max;
}

double __min__(DataFrame* df, int col)
{
    double min = GET(df, 0, col);

    for (int i = 1; i < df->rows; i++)
    {
        if (GET(df, i, col) <= min)
            min = GET(df, i, col);
    }
    
    return min;
}

double __mean__(DataFrame* df, int col)
{
    double sum = 0.0;
    for (int i = 0; i < df->rows; i++)
        sum += GET(df, i, col);
    
    return sum / df->rows;
}

// TODO: dato che per la mediana devo prendere il valore centrale
// è necessario sortare l'array. In questo modo ottengo in automatico la media, mediana, moda.
// Malloc e free una colona per volta, quick sort e calcolo.