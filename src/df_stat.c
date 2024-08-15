#include <stdlib.h>
#include <stdio.h>

#include "ccsv.h"
#include "df_stat.h"

// Build a statistical summary of the columns of the dataframe
Summary *__allocate_summary__(DataFrame *df, int *cols_skip, const int num_col_skip)
{
    Summary *summ = (Summary *)malloc(sizeof(Summary));
    summ->col = (int *)malloc(sizeof(double) * (df->cols - num_col_skip));
    summ->summary_min = (double *)malloc(sizeof(double) * (df->cols - num_col_skip));
    summ->summary_Q1 = (double *)malloc(sizeof(double) * (df->cols - num_col_skip));
    summ->summary_med = (double *)malloc(sizeof(double) * (df->cols - num_col_skip));
    summ->summary_mean = (double *)malloc(sizeof(double) * (df->cols - num_col_skip));
    summ->summary_Q3 = (double *)malloc(sizeof(double) * (df->cols - num_col_skip));
    summ->summary_max = (double *)malloc(sizeof(double) * (df->cols - num_col_skip));
    return summ;
}

// Free memory allocated by __allocate_summary__
void __free_summary__(Summary *summ)
{
    free(summ->col);
    free(summ->summary_min);
    free(summ->summary_Q1);
    free(summ->summary_med);
    free(summ->summary_mean);
    free(summ->summary_Q3);
    free(summ->summary_max);
    free(summ);
}

// TODO: dato che per la mediana devo prendere il valore centrale
// è necessario sortare l'array. In questo modo ottengo in automatico la media, mediana, moda.
// Malloc e free una colona per volta, quick sort e calcolo.

int __compare__(const void *a, const void *b)
{
    double arg1 = *(const double *)a;
    double arg2 = *(const double *)b;

    if (arg1 < arg2)
        return -1;
    if (arg1 > arg2)
        return 1;
    return 0;
}

void summary(Summary *summ, DataFrame *df, int *cols_skip, const int num_col_skip)
{
    double *array = (double *)malloc(sizeof(double) * df->rows);
    int k = 0;
    for (int i = 0; i < df->cols; i++)
    {
        // skip columns specified in cols_skip array
        for (int j = 0; j < num_col_skip; j++)
        {
            if (cols_skip[j] == i)
                goto skip;
        }

        double sum = 0.0;
        for (int j = 0; j < df->rows; j++)
        {
            array[j] = GET(df, j, i);
            sum += array[j];
        }
            
        qsort(array, df->rows, sizeof(double), __compare__);
        summ->col[k] = i;
        summ->summary_min[k] = array[0];
        summ->summary_max[k] = array[df->rows - 1];
        summ->summary_mean[k] = sum / df->rows;
        if (df->rows % 2 == 0)
            summ->summary_med[k] = 0.5 * (array[(df->rows - 1) / 2] + array[df->rows / 2]);
        else
            summ->summary_med[k] = array[df->rows / 2];

        k++;

    skip:
        continue;
    }

    free(array);
}

void printSummary(DataFrame *df, int *cols_skip, const int num_col_skip)
{
    Summary *summ = __allocate_summary__(df, cols_skip, num_col_skip);
    summary(summ, df, cols_skip, num_col_skip);

    for (int i = 0; i < df->cols - 1; i++)
    {
        printf("Column %d: \"%s\"\n", summ->col[i], df->head[summ->col[i]].nameCol);
        printf("Mean: %f\n", summ->summary_mean[i]);
        printf("Min: %f\n", summ->summary_min[i]);
        printf("Median: %f\n", summ->summary_med[i]);
        printf("Max: %f\n\n", summ->summary_max[i]);
    }
    __free_summary__(summ);
}