# kNN_MOF_SSIM
k-nearest resampling (kNN) and method-of-fragments (MOF) based rainfall temporal disaggregation model conditioned on Structural Similarity Index Measure (SSIM)

## 1. Introduction

A temporal disaggregation model is usually used to derive sub-daily (like hourly) distributions of weather variables from daily scale. kNN_MOF_SSIM is a non-parametric dtsaggregation procedure developed for multisite daily-to-hourly rainfall disaggregation. In the procedure, mothod of fragments (MOF) based on k-nearest neighbor resampling is applied. 

## 2. Techniques

### 2.1 Method Of Fragements

The idea of MOF is to resample a vector of fragments that represents the relative distribution of subdaily to daily rainfall (Pui et al., 2012). The number of fragments corresponds to the subdaily temporal resolution used, i.e. if the disaggregation is conducted from daily to hourly resolution, the relative distribution of subdaily values consists of 24 relative weights that sum up to 1

### 2.2 k-Nearest Neighbor algorithm

The k-nearest neighbors algorithm, also known as KNN or k-NN, is a non-parametric, supervised learning classifier, which uses proximity to make classifications or predictions about the grouping of an individual data point.

### 2.3 SSIM

Structural Similarity Index Measure (SSIM) is a perceptual metric originally developed for image quality assessment, which compares the similarity between two images by considering changes in structure, luminance, and contrast. Unlike traditional pixel-based metrics, such as Manhattan distance or Mean Squared Error (MSE), which only measure point-to-point differences, mSSIM focuses on the perceived visual quality by capturing structural information. The advantage of using mSSIM in the context of rainfall disaggregation is its ability to evaluate the spatial consistency and structural patterns of rainfall events across different sites. 

In this program, besides the SSIM-based metric, Manhattan distance can also be applied as the similarity measure in candidate sampling. 

## 3. How to use

### 3.1 Compile and run

- the program is written in c, with the compiler: gcc version 6.3.0 (MinGW.org GCC-6.3.0-1)
- CMake method: nevigate to the `scr` folder and you can find `CMakeLists.txt` file there and then call cmake tool `cmake .` to generate the necessary build files (one of them is `Makefile`), at last call `make` to produce the executable `kNN_MOF_SSIM.exe` (the name specified in `CMakeLists.txt`) in Windows OS or `kNN_MOF_SSIM` in Unix-like systems.
- `./kNN_MOF_cp.exe /path/to/gp.txt` to run the disaggregation, where `gp.txt` is an external file providing necessary arguments to run the program.

### 3.2 Global parameter file

`gp.txt` provides the key parameters controlling behaviors of the disaggregation, including file path and algorithm parameters. Detailed comments and explanation can be found in the example file `./data/gp.txt`.
Lines starting with the letter # are comment lines, ignored by the program.

## 4. Paper related

## 5. References

Pui, A., Sharma, A., Mehrotra, R., Sivakumar, B. and Jeremiah, E.  2012.  A comparison of alternatives for daily to sub-daily rainfall disaggregation. Journal of Hydrology, 470-471, 138-157. doi: https://doi.org/10.1016/j.jhydrol.2012.08.041.

Xiaoxiang Guan, Katrin Nissen, Viet Dung Nguyen, Bruno Merz, Benjamin Winter, Sergiy Vorogushyn. Multisite temporal rainfall disaggregation using methods of fragments conditioned on circulation patterns. Journal of Hydrology, 621, 129640. doi: https://doi.org/10.1016/j.jhydrol.2023.129640

## Author
[Xiaoxiang Guan](https://www.gfz-potsdam.de/staff/guan.xiaoxiang/sec44)

Email: guan@gfz-potsdam.de
