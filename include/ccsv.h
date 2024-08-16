#ifndef H_CCSV
#define H_CCSV

#include <stdio.h>
#include "ccsv.h"

#define MAX_LINE_LENGTH 1024
#define MAX_HEAD_SIZE 32
#define MAX_COLUMNS 64

#define CELL_SIZE 11

#define SEPARATOR ','

// i is the row and j the column
#define GET(df, i, j) df->data[i * df->cols + j]

typedef struct
{
    char nameCol[MAX_HEAD_SIZE]; 
} Head;

typedef struct
{
    Head *head;
    double *data;
    int rows;
    int cols;
} DataFrame;

DataFrame *readCSV(char* file_path);
void freeDataFrame(DataFrame* df);
void printDataFrame(DataFrame *df);

int __get_cols__(FILE* fileptr);
int __get_rows__(FILE* fileptr);
void __get_data__(FILE* fileptr, DataFrame* df);
void __get_header__(FILE *fileptr, DataFrame *df);

// Statistic 

typedef struct
{
    int* col; 
    double* summary_min;    // min value
    double* summary_Q1;     // first quartile
    double* summary_med;    // median
    double* summary_mean;   // mean
    double* summary_stdev;  // standard deviation
    double* summary_Q3;     // third quartile
    double* summary_max;    // max value
} Summary;


Summary* __allocate_summary__(DataFrame* df, int* cols_skip, const int num_col_skip);
void __free_summary__(Summary* summ);

int __compare__(const void *a, const void *b);
double __quartile__(double* arr, int n, int quartile);
void summary(Summary* summ, DataFrame* df, int* cols_skip, const int num_col_skip);
void printSummary(DataFrame *df, int *cols_skip, const int num_col_skip);
void correlationMatrix(DataFrame* df, Summary* summ, const int num_col_skip);

#endif