#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ccsv.h"

// Find the number of columns
int __get_cols__(FILE *fileptr)
{
    rewind(fileptr);            // reset file position
    char line[MAX_LINE_LENGTH]; // Line buffer
    int cols = 0;

    // Reading firs row
    if (fgets(line, MAX_LINE_LENGTH, fileptr) != NULL)
    {
        for (int i = 0; line[i] != '\0' && line[i] != '\n'; i++)
        {
            if (line[i] == ',')
                ++cols;
        }
        cols += 1; // Number of columns is number of commas + 1
    }

    return cols;
}

// Find the number of columns
int __get_rows__(FILE *fileptr)
{
    rewind(fileptr); // reset file position
    int rows = 0;
    int ch;
    int last_char = 0;

    // loop to the end of file
    while ((ch = fgetc(fileptr)) != EOF)
    {
        if (ch == '\n')
            rows++; // add 1 for each end of line
        last_char = ch;
    }

    // if the last charater isn't end of line, add 1 anyway
    if (last_char != '\n' && last_char != 0)
        rows++;

    return rows - 1;
}

// store data to dataframe
void __get_data__(FILE *fileptr, DataFrame *df)
{
    rewind(fileptr); // reset file position
    char line[MAX_LINE_LENGTH];

    // Skip first line (header)
    fgets(line, MAX_LINE_LENGTH, fileptr);

    int row = 0;
    while (fgets(line, MAX_LINE_LENGTH, fileptr))
    {
        int col = 0;
        char *cell = strtok(line, ",");
        while (cell != NULL && col < df->cols)
        {
            df->data[row * df->cols + col] = atof(cell);
            cell = strtok(NULL, ",");
            col++;
        }
        row++;
    }
}

// store data to dataframe
void __get_header__(FILE *fileptr, DataFrame *df)
{
    rewind(fileptr); // reset file position
    char line[MAX_LINE_LENGTH];

    if (fgets(line, MAX_LINE_LENGTH, fileptr) != NULL)
    {
        int i = 0;
        char *cell = strtok(line, ",");
        while (cell != NULL)
        {
            strncpy(df->head[i].nameCol, cell, MAX_HEAD_SIZE - 1);
            df->head[i].nameCol[MAX_HEAD_SIZE - 1] = '\0';
            cell = strtok(NULL, ",");
            i++;
        }
    }
}

// Allocate memory of the dataframe
DataFrame *readCSV(char *file_path)
{
    FILE *fileptr = fopen(file_path, "r");

    if (fileptr == NULL)
    {
        printf("ERROR: file not opened correctly\n");
        exit(1);
    }

    DataFrame *df = (DataFrame *)malloc(sizeof(DataFrame));

    df->cols = __get_cols__(fileptr);
    df->rows = __get_rows__(fileptr);
    df->head = (Head *)malloc(sizeof(Head) * df->cols);
    df->data = (double *)malloc(sizeof(double) * df->cols * df->rows);

    __get_data__(fileptr, df);
    __get_header__(fileptr, df);

    fclose(fileptr);

    return df;
}

// Free dataframe memory allocation
void freeDataFrame(DataFrame *df)
{
    free(df->head);
    free(df->data);
    free(df);
}


// Print dataframe
void printDataFrame(DataFrame *df)
{
    // Print upper border 
    printf("┌");
    for (int i = 1; i < CELL_SIZE * df->cols; i++)
    {
        if (i % CELL_SIZE == 0)
            printf("┬");
        else
            printf("─");
    }  
    printf("┐\n");

    // Print header
    printf("│ ");
    for (int i = 0; i < df->cols; i++)
        printf("%*.*s │ ", CELL_SIZE - 3, CELL_SIZE - 3, df->head[i].nameCol);

    printf("\n├");

    // Print header-data border
    for (int i = 1; i < CELL_SIZE * df->cols; i++)
    {
        if (i % CELL_SIZE == 0)
            printf("┼");
        else
            printf("─");
    }
    printf("┤\n");

    // Print data
    for (int i = 0; i < df->rows; i++)
    {
        printf("│");
        for (int j = 0; j < df->cols; j++)
        {
            printf(" %.*e │", CELL_SIZE - 9, GET(df, i, j));
        }
        printf("\n");
    }

    // Print lower border
    printf("└");
    for (int i = 1; i < CELL_SIZE * df->cols; i++)
    {
        if (i % CELL_SIZE == 0)
            printf("┴");
        else
            printf("─");
    }
    printf("┘\n");
}