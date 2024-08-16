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
    rewind(fileptr);        // reset file position
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

    // Skip first line (labeler)
    fgets(line, MAX_LINE_LENGTH, fileptr);

    int row = 0;
    while (fgets(line, MAX_LINE_LENGTH, fileptr))
    {
        int col = 0;
        char *cell = strtok(line, SEPARATOR);
        while (cell != NULL && col < df->cols)
        {
            df->data[row * df->cols + col] = atof(cell);
            cell = strtok(NULL, SEPARATOR);
            col++;
        }
        row++;
    }
}

// store labels to dataframe
void __get_label__(FILE *fileptr, DataFrame *df)
{
    rewind(fileptr); // reset file position
    char line[MAX_LINE_LENGTH];

    // Copy string from csv to label array
    if (fgets(line, MAX_LINE_LENGTH, fileptr) != NULL)
    {
        int i = 0;
        char *cell = strtok(line, SEPARATOR); 

        while (cell != NULL)
        {
            strncpy(df->label[i].nameCol, cell, MAX_LABEL_SIZE - 1);
            df->label[i].nameCol[MAX_LABEL_SIZE - 1] = '\0';

            cell = strtok(NULL, SEPARATOR);
            i++;
        }
    }

    // Control if last label has more than one new line
    for (int j = 0; j < MAX_LABEL_SIZE - 2; j++)
    {
        if (df->label[df->cols - 1].nameCol[j] == '\n' || df->label[df->cols - 1].nameCol[j] == '\r')
            df->label[df->cols - 1].nameCol[j] = 0;
    }
}

// Memory allocation
DataFrame * __allocate_DataFrame__(int rows, int cols)
{
    DataFrame *df = (DataFrame *)malloc(sizeof(DataFrame));
    df->cols = cols;
    df->rows = rows;
    df->data = (double *)malloc(sizeof(double) * rows * cols);
    df->label = (Label *)malloc(sizeof(Label) * cols);

    return df;
}

// Free memory of dataframe
void freeDataFrame(DataFrame *df)
{
    free(df->data);
    free(df->label);
    __free_summary__(df);
    free(df);
}

// Load csv file to dataframe structure
DataFrame* read_csv(char *file_path)
{
    FILE *fileptr = fopen(file_path, "r");
    if (fileptr == NULL)
    {
        printf("ERROR: file not opened correctly\n");
        exit(1);
    }
    
    DataFrame *df = __allocate_DataFrame__( __get_rows__(fileptr), __get_cols__(fileptr));
    __get_data__(fileptr, df);
    __get_label__(fileptr, df);

    fclose(fileptr);

    return df;
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
        printf("%*.*s │ ", CELL_SIZE - 3, CELL_SIZE - 3, df->label[i].nameCol);

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
            printf(" %+.*e │", CELL_SIZE - 10, GET(df, i, j));
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

// Return the name of the column
char* colname(DataFrame *df, int col)
{
    return df->label[col].nameCol;
}

// Print all the names of the columns
void printColsName(DataFrame *df)
{
    printf("[");
    for (int i = 0; i < df->cols - 1; i++)
        printf("'%s', ", df->label[i].nameCol);
    printf("'%s']\n", df->label[df->cols - 1].nameCol);
}