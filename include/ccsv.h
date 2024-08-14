#ifndef H_CCSV
#define H_CCSV

#include <stdio.h>

#define MAX_LINE_LENGTH 1024
#define MAX_HEAD_SIZE 16
#define MAX_COLUMNS 64

#define CELL_SIZE 12

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

#endif