#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "ccsv.h"

// Build a statistical summary of the columns of the dataframe
Summary *__allocate_summary__(DataFrame *df, int *cols_skip, const int num_col_skip)
{
    Summary *summ = (Summary *)malloc(sizeof(Summary));
    summ->col = (int *)malloc(sizeof(double) * (df->cols - num_col_skip));
    summ->summary_min = (double *)malloc(sizeof(double) * (df->cols - num_col_skip));
    summ->summary_Q1 = (double *)malloc(sizeof(double) * (df->cols - num_col_skip));
    summ->summary_med = (double *)malloc(sizeof(double) * (df->cols - num_col_skip));
    summ->summary_mean = (double *)malloc(sizeof(double) * (df->cols - num_col_skip));
    summ->summary_stdev = (double *)malloc(sizeof(double) * (df->cols - num_col_skip));
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
    free(summ->summary_stdev);
    free(summ->summary_mean);
    free(summ->summary_Q3);
    free(summ->summary_max);
    free(summ);
}

// Compare function for quick sort
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

// Calculate quartile
double __quartile__(double* arr, int n, int quartile)
{
    double pos = (quartile * (n + 1)) / 4.0;
    int lowerIndex = (int)pos - 1;
    double fractionalPart = pos - (int)pos;

    if (fractionalPart == 0) {
        return arr[lowerIndex];
    } else {
        return arr[lowerIndex] + fractionalPart * (arr[lowerIndex + 1] - arr[lowerIndex]);
    }
}

// Calculate statistics of the columns
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

        // save column in to an array and compute the sum
        double sum = 0.0;
        double sum_square = 0.0;
        for (int j = 0; j < df->rows; j++)
        {
            array[j] = GET(df, j, i);
            sum += array[j];
            sum_square += array[j] * array[j];
        }
        
        // sort of the array
        qsort(array, df->rows, sizeof(double), __compare__);
        summ->col[k] = i;
        summ->summary_min[k] = array[0];
        summ->summary_max[k] = array[df->rows - 1];
        summ->summary_mean[k] = sum / df->rows;
        summ->summary_stdev[k] = sqrt((sum_square / df->rows - summ->summary_mean[k] * summ->summary_mean[k]) * (df->rows) / (df->rows - 1.0));
        summ->summary_Q1[k] = __quartile__(array, df->rows, 1);
        summ->summary_med[k] = __quartile__(array, df->rows, 2);
        summ->summary_Q3[k] = __quartile__(array, df->rows, 3);
        k++;

    skip:
        continue;
    }

    free(array);
}

// Print summary statistics
void printSummary(DataFrame *df, int *cols_skip, const int num_col_skip)
{
    Summary *summ = __allocate_summary__(df, cols_skip, num_col_skip);
    summary(summ, df, cols_skip, num_col_skip);

    for (int i = 0; i < df->cols - 1; i++)
    {
        printf("┌───────────────────────────┐\n");
        printf("│Column %2d: %16.16s│\n", summ->col[i], df->head[summ->col[i]].nameCol);
        printf("├───────────────────────────┤\n");
        printf("│Mean:       \t: %+.3e│\n", summ->summary_mean[i]);
        printf("│Std. Dev.   \t: %+.3e│\n", summ->summary_stdev[i]);
        printf("├───────────────────────────┤\n");
        printf("│Min    \t: %+.3e│\n", summ->summary_min[i]);
        printf("│1st Qu.\t: %+.3e│\n", summ->summary_Q1[i]);
        printf("│Median \t: %+.3e│\n", summ->summary_med[i]);
        printf("│3rd Qu.\t: %+.3e│\n", summ->summary_Q3[i]);
        printf("│Max.   \t: %+.3e│\n", summ->summary_max[i]);
        printf("└───────────────────────────┘\n");
    }
    __free_summary__(summ);
}

// Pearson correlation matrix
void correlationMatrix(DataFrame* df, Summary* summ, const int num_col_skip)
{
    int n = df->cols - num_col_skip;
    double* corrMat = (double *)malloc(sizeof(double) * (n * n));   // correlation matrix n x n

    // calculate lower matrix correlatio
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < i; j++)
        {
            double corrij = 0.0;
            for (int k = 0; k < df->rows; k++)
                corrij += (GET(df, k, summ->col[i]) - summ->summary_mean[i]) * (GET(df, k, summ->col[j]) - summ->summary_mean[j]);

            corrMat[i * n + j] = corrij / ((df->rows - 1) * summ->summary_stdev[i] * summ->summary_stdev[j]);
        }
    }

    // simmetrize correlation amtrix
    for (int i = 0; i < n; i++)
    {
        for (int j = i; j < n; j++)
            corrMat[i * n + j] = corrMat[j * n + i];

        corrMat[i * n + i] = 1.0;
    }

    //--- print matrix -----

    // Print upper border
    printf("┌");
    for (int i = 1; i < CELL_SIZE * (n + 1); i++)
    {
        if (i % CELL_SIZE == 0)
            printf("┬");
        else
            printf("─");
    }
    printf("┐\n");

    // Print header
    printf("│%*.*s|", CELL_SIZE - 1, CELL_SIZE - 1, " ");
    for (int i = 0; i < n; i++)
        printf(" %*.*s │", CELL_SIZE - 3, CELL_SIZE - 3, df->head[summ->col[i]].nameCol);

    printf("\n├");

    // Print header-data border
    for (int i = 1; i < CELL_SIZE * (n + 1); i++)
    {
        if (i % CELL_SIZE == 0)
            printf("┼");
        else
            printf("─");
    }
    printf("┤\n");
    
    // print data
    for (int i = 0; i < n; i++)
    {
        // print header 
        printf("│%*.*s│", CELL_SIZE - 1, CELL_SIZE - 1, df->head[summ->col[i]].nameCol);
        for (int j = 0; j < n; j++)
            printf("   %+.*f  │", CELL_SIZE - 9,  corrMat[i * n + j]);

        if (i != n - 1)
        {
            printf("\n├");
            for (int j = 1; j < CELL_SIZE * (n + 1); j++)
            {
                if (j % CELL_SIZE == 0)
                    printf("┼");
                else
                    printf("─");
            }
            printf("┤\n");
        }
    }
    
    // Print lower border
    printf("\n└");
    for (int i = 1; i < CELL_SIZE * (n + 1); i++)
    {
        if (i % CELL_SIZE == 0)
            printf("┴");
        else
            printf("─");
    }
    printf("┘\n");

    free(corrMat);
}