/*
 * SUMMARY:      Func_SSIM.c
 * USAGE:        the main algorithm of Structural Similarity Index Measure
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Apr-2024
 * DESCRIPTION:  compuate the SSIM between two images. 
 *               The SSIM represents how close the two images are to each other.
 * DESCRIP-END.
 * FUNCTIONS:    meanSSIM(); mean(); StandardDeviation(); covariance()
 *               isNODATA();
 * 
 * COMMENTS:
 * 
 * REFERENCEs:
 * All about Structural Similarity Index (SSIM): Theory + Code in PyTorch
 *      https://medium.com/srm-mic/all-about-structural-similarity-index-ssim-theory-code-in-pytorch-6551b455541e
 * Wikipedia: https://en.wikipedia.org/wiki/Structural_similarity_index_measure
 * 
 */

/*******************************************************************************
 * VARIABLEs:
 * double *image                      - 1D double-type array for rainfall at multiple sites
 * double NODATA                      - the value of NODATA
 * int size                           - number of rain sites within the domain
 * double L                           - the maximum value in the rainfall images
 * double *k                          - parameters in SSIM algorithm, 3-elements array
 * double *power                      - 3 power parameters in SSIM algorithm, 3-elements array
 * 
 * 
********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Func_SSIM.h"


double meanSSIM(
    double *image1,
    double *image2,
    double NODATA,
    int size,
    double *k,
    double *power
)
{
    double L;
    L = SSIM_L(image1, image2, NODATA, size);

    int counts = 0;
    double image1_mean, image2_mean;
    double image1_sd, image2_sd; 
    double image_cov;
    image1_mean = mean(image1, NODATA, size);
    image2_mean = mean(image2, NODATA, size);
    image1_sd = StandardDeviation(image1, image1_mean, NODATA, size);
    image2_sd = StandardDeviation(image2, image2_mean, NODATA, size);
    image_cov = covariance(image1, image2, image1_mean, image2_mean, NODATA, size);

    // printf("image1_mean: %f,image2_mean: %f,image1_sd: %f,image2_sd: %f,image_cov: %f\n",
    //        image1_mean, image2_mean, image1_sd, image2_sd, image_cov);
    double SSIM_l, SSIM_c, SSIM_s, SSIM;
    double C[3] = {0, 0, 0};
    for (size_t i = 0; i < 3; i++)
    {
        C[i] = pow(*(k + i) * L, 2);
    }
    // printf("L:%f, C1:%f, C2:%f, C3:%f\n", L, C[0], C[1], C[2]);

    SSIM_l = (2 * image1_mean * image2_mean + C[0]) / (pow(image1_mean, 2) + pow(image2_mean, 2) + C[0]);
    SSIM_c = (2 * image1_sd * image2_sd + C[1]) / (pow(image1_sd, 2) + pow(image2_sd, 2) + C[1]);
    SSIM_s = (image_cov + C[2]) / (image1_sd * image2_sd + C[2]);
    SSIM = pow(SSIM_l, *(power + 0)) * pow(SSIM_c, *(power + 1)) * pow(SSIM_s, *(power + 2));
    // printf("SSIM_l:%f, SSIM_c:%f, SSIM_s:%f, SSIM:%f\n", SSIM_l, SSIM_c, SSIM_s, SSIM);
    return SSIM;
}

double mean(
    double *image,
    double NODATA,
    int size
)
{
    int counts = 0;
    double sum = 0.0;
    for (size_t i = 0; i < size; i++)
    {
        if (isNODATA(*(image + i), NODATA) == 0)
        {
            counts += 1;
            sum += *(image + i);
        }
    }
    if (counts == 0)
    {
        printf("NULL: an empty image is detected!\n");
        exit(1);
    }
    return (sum / (double) counts);
}

double StandardDeviation(
    double *image,
    double image_mean,
    double NODATA,
    int size
)
{
    int counts = 0;
    double square_sum = 0.0;
    for (size_t i = 0; i < size; i++)
    {
        if (isNODATA(*(image + i), NODATA) == 0)
        {
            counts += 1;
            square_sum += pow((*(image + i) - image_mean), 2);
        }
    }
    if (counts <= 1)
    {
        printf("NULL: an empty image is detected!\n");
        exit(1);
    }
    return (pow(1 / ((double) counts - 1) * square_sum, 0.5));
}

double covariance(
    double *image1,
    double *image2,
    double image1_mean,
    double image2_mean,
    double NODATA,
    int size
)
{
    int counts = 0;
    double sum = 0.0;
    for (size_t i = 0; i < size; i++)
    {
        if (isNODATA(*(image1 + i), NODATA) == 0)
        {
            counts += 1;
            sum += (*(image1 + i) - image1_mean) * (*(image2 + i) - image2_mean);
        }
    }
    if (counts <= 1)
    {
        printf("NULL: an empty image is detected!\n");
        exit(1);
    }
    return (1 / ((double) counts - 1) * sum);
}

int isNODATA(
    double x,
    double NODATA
)
{
    double delta = 0.01;
    if (x >= (NODATA - delta) && x <= (NODATA + delta))
    {
        // x is close enough to NODATA
        return 1;
    } else {
        return 0;
    }
}

double SSIM_L(
    double *image1,
    double *image2,
    double NODATA,
    int size
)
{
    double L = 0.0;
    for (size_t i = 0; i < size; i++)
    {
        if (*(image1 + i) > L)
        {
            L = *(image1 + i);
        }
        if (*(image2 + i) > L)
        {
            L = *(image2 + i);
        }
    }
    return L;
}