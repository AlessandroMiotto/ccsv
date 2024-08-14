#ifndef H_CCSV
#define H_CCSV

#define MAX_LINE_LENGTH 1024
#define MAX_HEAD_SIZE 16
#define MAX_COLUMNS 64

#define SEPARATOR ','

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

int __get_cols__(FILE* fileptr);
int __get_rows__(FILE* fileptr);
void __get_data__(FILE* fileptr, DataFrame* df);
void __get_header__(FILE *fileptr, DataFrame *df);

#endif