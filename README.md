# CCSV

CCSV is a simple CSV parser written in pure C with the goal of mimic [R](https://www.r-project.org/) and [pandas](https://pandas.pydata.org/) dataframe. It offers dataframe structure and automatic descriptive statistis. For now, CCSV can handle only numerical dataset.

---

Usage example
--------------
This example shows how to import a csv file and display statistical quantities in a table format.
```c
#include "ccsv.h"

int main()
{
    // Create dataframe reading data from csv file
    DataFrame *df = read_csv("file.csv");

    // Print all the dataframe
    printDataFrame(df);

    // Array of columns to skip
    // In this example first row is used for IDs
    int col_skip[1] = {0};
    
    // Print statistical summary
    printSummary(df, skip, 1);

    // Print correlation matrix
    corr(df);

    // Deallocate dataframe
    freeDataFrame(df);

    return 0;
}
```

Build from source
--------------
CCSV use make as build system and OpenMP for multithreading. 

1. Clone the repository 
    ```
    git clone https://github.com/AlessandroMiotto/ccsv/
    ```
2. Compile and run using make. `make` will create `./build` directory with the executable inside.
    ```
    cd csv/
    make
    make run
    ```
    For clean `./build` directory, run `make clean`

ToDo list
--------------
- [x] Statistical summary
- [x] Correlation analysis
- [ ] Regression models
- [ ] Histogram
- [x] Multithreading support
