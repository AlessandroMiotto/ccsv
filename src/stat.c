#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <omp.h>

#include "ccsv.h"

void __allocate_summary__(DataFrame *df, const int *cols_skip, const int num_col_skip)
{
    df->col_skip_num = num_col_skip;
    df->col_index = (int *)malloc(sizeof(int) * (df->cols - num_col_skip));
    df->stats = (Stats *)malloc(sizeof(Stats) * (df->cols - num_col_skip));
}

void __free_summary__(DataFrame *df)
{
    free(df->col_index);
    free(df->stats);
    df->stats = NULL;
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
double __quartile__(double *arr, int n, const int quartile)
{
    double pos = (quartile * (n + 1)) / 4.0;
    int lowerIndex = (int)pos - 1;
    double fractionalPart = pos - (int)pos;

    if (fractionalPart == 0)
    {
        return arr[lowerIndex];
    }
    else
    {
        return arr[lowerIndex] + fractionalPart * (arr[lowerIndex + 1] - arr[lowerIndex]);
    }
}

// Calculate statistics of the columns
void __summary__(DataFrame *df, const int *cols_skip, const int num_col_skip)
{
    __free_summary__(df);
    __allocate_summary__(df, cols_skip, num_col_skip);

    double *array = (double *)malloc(sizeof(double) * df->rows);
    int k = 0;

    for (int i = 0; i < df->cols; i++)
    {
        // skip columns specified in cols_skip array
        for (int j = 0; j < df->col_skip_num; j++)
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
        df->col_index[k] = i;
        df->stats[k].min = array[0];
        df->stats[k].max = array[df->rows - 1];
        df->stats[k].mean = sum / df->rows;
        df->stats[k].stdev = sqrt((sum_square / df->rows - df->stats[k].mean * df->stats[k].mean) * (df->rows) / (df->rows - 1.0));
        df->stats[k].Q1 = __quartile__(array, df->rows, 1);
        df->stats[k].median = __quartile__(array, df->rows, 2);
        df->stats[k].Q3 = __quartile__(array, df->rows, 3);
        k++;

    skip:
        continue;
    }

    free(array);
}

// Print summary statistics
void printSummary(DataFrame *df, const int *cols_skip, const int num_col_skip)
{
    if (df->stats == NULL || num_col_skip != df->col_skip_num)
        __summary__(df, cols_skip, num_col_skip);

    for (int i = 0; i < df->cols - num_col_skip; i++)
    {
        printf("┌───────────────────────────┐\n");
        printf("│Column %2d: %16.16s│\n", df->col_index[i], df->label[df->col_index[i]].nameCol);
        printf("├───────────────────────────┤\n");
        printf("│Mean:       \t: %+.3e│\n", df->stats[i].mean);
        printf("│Std. Dev.   \t: %+.3e│\n", df->stats[i].stdev);
        printf("├───────────────────────────┤\n");
        printf("│Min    \t: %+.3e│\n", df->stats[i].min);
        printf("│1st Qu.\t: %+.3e│\n", df->stats[i].Q1);
        printf("│Median \t: %+.3e│\n", df->stats[i].median);
        printf("│3rd Qu.\t: %+.3e│\n", df->stats[i].Q3);
        printf("│Max.   \t: %+.3e│\n", df->stats[i].max);
        printf("└───────────────────────────┘\n");
    }
}

// Utils for line printing
void __print_line__(int n, const char *left, const char *middle, const char *right)
{
    printf("%s", left);
    for (int i = 1; i < CELL_SIZE * (n + 1); i++)
    {
        if (i % CELL_SIZE == 0)
            printf("%s", middle);
        else
            printf("─");
    }
    printf("%s\n", right);
}

// Pearson correlation matrix
void corr(DataFrame *df, const int *cols_skip, const int num_col_skip)
{
    if (df->stats == NULL || num_col_skip != df->col_skip_num)
        __summary__(df, cols_skip, num_col_skip);

    int n = df->cols - df->col_skip_num;
    double *corrMat = (double *)malloc(sizeof(double) * (n * n)); // correlation matrix n x n

    // calculate lower matrix correlatio
    #pragma omp parallel for num_threads(NUM_THREADS)
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < i; j++)
        {
            double corrij = 0.0;
            for (int k = 0; k < df->rows; k++)
                corrij += (GET(df, k, df->col_index[i]) - df->stats[i].mean) * (GET(df, k, df->col_index[j]) - df->stats[j].mean);

            corrMat[i * n + j] = corrij / ((df->rows - 1) * df->stats[i].stdev * df->stats[j].stdev);
        }
    }

    // simmetrize correlation amtrix
    #pragma omp parallel for num_threads(NUM_THREADS)
    for (int i = 0; i < n; i++)
    {
        #pragma omp parallel for num_threads(NUM_THREADS)
        for (int j = i; j < n; j++)
            corrMat[i * n + j] = corrMat[j * n + i];

        corrMat[i * n + i] = 1.0;
    }

    //--- print matrix -----

    // Print upper border
    __print_line__(n, "┌", "┬", "┐");

    // Print header
    printf("│%*.*s|", CELL_SIZE - 1, CELL_SIZE - 1, " ");
    for (int i = 0; i < n; i++)
        printf(" %*.*s │", CELL_SIZE - 3, CELL_SIZE - 3, df->label[df->col_index[i]].nameCol);

    printf("\n");

    // Print header-data border
    __print_line__(n, "├", "┼", "┤");

    // print data
    for (int i = 0; i < n; i++)
    {
        // print header
        printf("│ %*.*s │", CELL_SIZE - 3, CELL_SIZE - 3, df->label[df->col_index[i]].nameCol);
        for (int j = 0; j < n; j++)
            printf("   %+.*f  │", CELL_SIZE - 9, corrMat[i * n + j]);

        if (i != n - 1)
        {
            printf("\n");
            __print_line__(n, "├", "┼", "┤");
        }
    }

    // Print lower border
    printf("\n");
    __print_line__(n, "└", "┴", "┘");

    free(corrMat);
}

// Return the mean of the column col
double mean(DataFrame *df, int col)
{
    // if stats was computed
    if (df->stats != NULL)
    {
        for (int i = 0; i < df->cols - df->col_skip_num; i++)
        {
            if (df->col_index[i] == col)
                return df->stats[i].mean;
        }
    }

    // if stat was not computed
    double sum = 0.0;

    for (int i = 0; i < df->rows; i++)
        sum += GET(df, i, col);

    return sum / df->rows;
}

// Return the standard deviation of the column col
double std(DataFrame *df, int col)
{
    // if stats was computed
    if (df->stats != NULL)
    {
        for (int i = 0; i < df->cols - df->col_skip_num; i++)
        {
            if (df->col_index[i] == col)
                return df->stats[i].stdev;
        }
    }

    // if stat was not computed
    double sum_square = 0.0;
    double mean_val = mean(df, col);

    for (int i = 0; i < df->rows; i++)
        sum_square += GET(df, i, col) * GET(df, i, col);

    return sqrt((sum_square / df->rows - mean_val * mean_val) * (df->rows) / (df->rows - 1.0));
}

// Return the median of the column col
double median(DataFrame *df, int col)
{
    // if stats was computed
    if (df->stats != NULL)
    {
        for (int i = 0; i < df->cols - df->col_skip_num; i++)
        {
            if (df->col_index[i] == col)
                return df->stats[i].median;
        }
        
        printf("ERROR: column number not valid. Maybe it was skipped previously\n");
        exit(1);
    }

    printf("ERROR: statistical report was not computed yet\n");
    exit(1);
}

// Return the max of the column col
double max(DataFrame *df, int col)
{
     // if stats was computed
    if (df->stats != NULL)
    {
        for (int i = 0; i < df->cols - df->col_skip_num; i++)
        {
            if (df->col_index[i] == col)
                return df->stats[i].max;
        }

        printf("ERROR: column number not valid. Maybe it was skipped previously\n");
        exit(1);
    }

    printf("ERROR: statistical report was not computed yet\n");
    exit(1);
    
}

// Return the min of the column col
double min(DataFrame *df, int col)
{
     // if stats was computed
    if (df->stats != NULL)
    {
        for (int i = 0; i < df->cols - df->col_skip_num; i++)
        {
            if (df->col_index[i] == col)
                return df->stats[i].min;
        }

        printf("ERROR: column number not valid. Maybe it was skipped previously\n");
        exit(1);
    }

    printf("ERROR: statistical report was not computed yet\n");
    exit(1);
}