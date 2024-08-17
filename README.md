# CCSV

CCSV is a simple CSV parser written in pure C with the goal of mimic [R](https://www.r-project.org/) and [pandas](https://pandas.pydata.org/) dataframe. CCSV offers dataframe structure, descriptive statistis reports and linear regression model using least square method. For now, CCSV can handle only numerical dataset.

---

Usage example
--------------
This example shows how to import a csv file, display statistical quantities and compute linear regression.
```c
#include "ccsv.h"
#include "regression.h"

int main()
{
    // Create dataframe reading data from csv file
    DataFrame *df = read_csv("file.csv");

    // Print all the dataframe
    printDataFrame(df, ALL_LINE);

    // ===== DESCRIPTIVE STATISTICs =====
    // Array of columns to skip
    // In this example first row is used for IDs
    int col_skip[1] = {0};
    
    // Print statistical summary
    printSummary(df, skip, 1);

    // Print correlation matrix
    corr(df);

    // ======= LINEAR REGRESSION =======
    // Array of linear regression coefficients
    double beta[8];

    // Linear regression model specifications
    LinearRegression model = {
        .y_col = 8,         // response column index
        .beta = beta,       // array of coefficients

        // dimensionality of the regression
        .x_cols = (int[]){1, 2, 3, 4, 5, 6, 7}, 
        .x_cols_num = 7,

        // true for print results 
        .print = true
    };

    // Compute linear regression
    linearRegression(df, model);

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
- [x] Regression models
- [ ] Add MKL support
- [ ] Histograms
- [x] Multithreading support
