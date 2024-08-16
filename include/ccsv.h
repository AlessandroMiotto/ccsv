#ifndef H_CCSV
#define H_CCSV

#include <stdio.h>
#include "ccsv.h"

#define NUM_THREADS 8

#define MAX_LINE_LENGTH 1024
#define MAX_COLUMNS 64
#define MAX_LABEL_SIZE 32
#define CELL_SIZE 12

#define SEPARATOR ","

// i is the row and j is the column
#define GET(df, i, j) df->data[i * df->cols + j]

typedef struct
{
    // name of a column
    char nameCol[MAX_LABEL_SIZE]; 
} Label;

typedef struct
{
    double mean;
    double stdev;

    double min;
    double Q1;
    double median;
    double Q3;
    double max;
} Stats;

typedef struct
{
    Label *label;        // array of column names

    int col_skip_num;   // number of skipped columns
    int *col_index;     // array of keeped columns
    Stats *stats;       // array of columns statistics

    double *data;
    int rows;
    int cols;
} DataFrame;


int __get_cols__(FILE *fileptr);
int __get_rows__(FILE *fileptr);
void __get_data__(FILE *fileptr, DataFrame *df);
void __get_label__(FILE *fileptr, DataFrame *df);
DataFrame * __allocate_DataFrame__(int rows, int cols);

void freeDataFrame(DataFrame *df);
DataFrame* read_csv(char *file_path);
void printDataFrame(DataFrame *df);
char* colname(DataFrame *df, int col);
void printColsName(DataFrame *df);

void __allocate_summary__(DataFrame *df, int *cols_skip, const int num_col_skip);
void __free_summary__(DataFrame *df);
int __compare__(const void *a, const void *b);
double __quartile__(double* arr, int n, const int quartile);
void __summary__(DataFrame *df, int *cols_skip, const int num_col_skip);
void __print_line__(int n, const char* left, const char* middle, const char* right);

void printSummary(DataFrame *df, int *cols_skip, const int num_col_skip);
void corr(DataFrame* df);

double mean(DataFrame *df, int col);
double sdt(DataFrame *df, int col);
double median(DataFrame *df, int col);
double max(DataFrame *df, int col);
double min(DataFrame *df, int col);

#endif