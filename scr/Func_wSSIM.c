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
 * weighted structural similarity index measure (wSSIM):
 * - gaussian weight function
 * - exponential decay weight function
 * in the weight functions, parameters (like mean, standard deviation) are 
 * estimated from one image (in this app, the rainfall map for target day)
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
#include "Func_wSSIM.h"


double weightSSIM_Gaussian(
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
    double gaussian_mu, gaussian_sigma;
    // assume that the image1 is from the target day
    gaussian_mu = mean(image1, NODATA, size);
    gaussian_sigma = StandardDeviation(image1, gaussian_mu, NODATA, size);

    double image1_mean, image2_mean;
    double image1_sd, image2_sd; 
    double image_cov;
    image1_mean = mean_Weight_Gaussian(image1, NODATA, size, gaussian_mu, gaussian_sigma);
    image2_mean = mean_Weight_Gaussian(image2, NODATA, size, gaussian_mu, gaussian_sigma);
    image1_sd = SD_Weight_Gaussian(image1, image1_mean, NODATA, size, gaussian_mu, gaussian_sigma);
    image2_sd = SD_Weight_Gaussian(image2, image2_mean, NODATA, size, gaussian_mu, gaussian_sigma);
    image_cov = CV_Weight_Gaussian(image1, image2, image1_mean, image2_mean, NODATA, size, gaussian_mu, gaussian_sigma);

    double SSIM_l, SSIM_c, SSIM_s, wSSIM;
    double C[3] = {0, 0, 0};  // constants 
    for (size_t i = 0; i < 3; i++)
    {
        C[i] = pow(*(k + i) * L, 2);
    }

    SSIM_l = (2 * image1_mean * image2_mean + C[0]) / (pow(image1_mean, 2) + pow(image2_mean, 2) + C[0]);
    SSIM_c = (2 * image1_sd * image2_sd + C[1]) / (pow(image1_sd, 2) + pow(image2_sd, 2) + C[1]);
    SSIM_s = (image_cov + C[2]) / (image1_sd * image2_sd + C[2]);
    wSSIM = pow(SSIM_l, *(power + 0)) * pow(SSIM_c, *(power + 1)) * pow(SSIM_s, *(power + 2));

    return wSSIM;
}

double mean_Weight_Gaussian(
    double *image,
    double NODATA,
    int size,
    double gaussian_mu,
    double gaussian_sigma
)
{
    int counts = 0;
    double sum = 0.0;
    double w = 0.0;  // the weight
    for (size_t i = 0; i < size; i++)
    {
        if (isNODATA(*(image + i), NODATA) == 0)
        {
            counts += 1;
            w = exp(1 - pow(*(image + i) - gaussian_mu, 2) / 2 / pow(gaussian_sigma,2));
            sum += *(image + i) * w;
        }
    }
    if (counts == 0)
    {
        printf("NULL: an empty image is detected!\n");
        exit(1);
    }
    return (sum / (double) counts);
}


double SD_Weight_Gaussian(
    double *image,
    double image_mean,
    double NODATA,
    int size,
    double gaussian_mu,
    double gaussian_sigma
)
{
    int counts = 0;
    double square_sum = 0.0;
    double w = 0.0;
    for (size_t i = 0; i < size; i++)
    {
        if (isNODATA(*(image + i), NODATA) == 0)
        {
            counts += 1;
            w = exp(1 - pow(*(image + i) - gaussian_mu, 2) / 2 / pow(gaussian_sigma,2));
            square_sum += pow((*(image + i) - image_mean), 2) * w;
        }
    }
    if (counts <= 1)
    {
        printf("NULL: an empty image is detected!\n");
        exit(1);
    }
    return (pow(1 / ((double) counts - 1) * square_sum, 0.5));
}


double CV_Weight_Gaussian(
    double *image1,  // target
    double *image2,  // candidate
    double image1_mean,
    double image2_mean,
    double NODATA,
    int size,
    double gaussian_mu,
    double gaussian_sigma
)
{
    int counts = 0;
    double sum = 0.0;
    double w;
    for (size_t i = 0; i < size; i++)
    {
        if (isNODATA(*(image1 + i), NODATA) == 0)
        {
            counts += 1;
            w = exp(1 - pow(*(image1 + i) - gaussian_mu, 2) / 2 / pow(gaussian_sigma,2));
            sum += (*(image1 + i) - image1_mean) * (*(image2 + i) - image2_mean) * w;
        }
    }
    if (counts <= 1)
    {
        printf("NULL: an empty image is detected!\n");
        exit(1);
    }
    return (1 / ((double) counts - 1) * sum);
}


/**********************
 * exponential decay weight function
 * **********************/ 


double weightSSIM_ExpoDecay(
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
    double ExpoDecay_mu, ExpoDecay_sigma;
    // assume that the image1 is from the target day
    ExpoDecay_mu = mean(image1, NODATA, size);
    ExpoDecay_sigma = StandardDeviation(image1, ExpoDecay_mu, NODATA, size);

    double image1_mean, image2_mean;
    double image1_sd, image2_sd; 
    double image_cov;
    image1_mean = mean_Weight_Gaussian(image1, NODATA, size, ExpoDecay_mu, ExpoDecay_sigma);
    image2_mean = mean_Weight_Gaussian(image2, NODATA, size, ExpoDecay_mu, ExpoDecay_sigma);
    image1_sd = SD_Weight_Gaussian(image1, image1_mean, NODATA, size, ExpoDecay_mu, ExpoDecay_sigma);
    image2_sd = SD_Weight_Gaussian(image2, image2_mean, NODATA, size, ExpoDecay_mu, ExpoDecay_sigma);
    image_cov = CV_Weight_Gaussian(image1, image2, image1_mean, image2_mean, NODATA, size, ExpoDecay_mu, ExpoDecay_sigma);

    double SSIM_l, SSIM_c, SSIM_s, wSSIM;
    double C[3] = {0, 0, 0};  // constants 
    for (size_t i = 0; i < 3; i++)
    {
        C[i] = pow(*(k + i) * L, 2);
    }

    SSIM_l = (2 * image1_mean * image2_mean + C[0]) / (pow(image1_mean, 2) + pow(image2_mean, 2) + C[0]);
    SSIM_c = (2 * image1_sd * image2_sd + C[1]) / (pow(image1_sd, 2) + pow(image2_sd, 2) + C[1]);
    SSIM_s = (image_cov + C[2]) / (image1_sd * image2_sd + C[2]);
    wSSIM = pow(SSIM_l, *(power + 0)) * pow(SSIM_c, *(power + 1)) * pow(SSIM_s, *(power + 2));

    return wSSIM;
}

double mean_Weight_ExpoDecay(
    double *image,
    double NODATA,
    int size,
    double ExpoDecay_mu,
    double ExpoDecay_sigma
)
{
    int counts = 0;
    double sum = 0.0;
    double w = 0.0;  // the weight
    for (size_t i = 0; i < size; i++)
    {
        if (isNODATA(*(image + i), NODATA) == 0)
        {
            counts += 1;
            w = exp(ExpoDecay_sigma * abs(*(image + i) - ExpoDecay_mu));
            sum += *(image + i) * w;
        }
    }
    if (counts == 0)
    {
        printf("NULL: an empty image is detected!\n");
        exit(1);
    }
    return (sum / (double) counts);
}


double SD_Weight_ExpoDecay(
    double *image,
    double image_mean,
    double NODATA,
    int size,
    double ExpoDecay_mu,
    double ExpoDecay_sigma
)
{
    int counts = 0;
    double square_sum = 0.0;
    double w = 0.0;
    for (size_t i = 0; i < size; i++)
    {
        if (isNODATA(*(image + i), NODATA) == 0)
        {
            counts += 1;
            w = exp(ExpoDecay_sigma * abs(*(image + i) - ExpoDecay_mu));
            square_sum += pow((*(image + i) - image_mean), 2) * w;
        }
    }
    if (counts <= 1)
    {
        printf("NULL: an empty image is detected!\n");
        exit(1);
    }
    return (pow(1 / ((double) counts - 1) * square_sum, 0.5));
}


double CV_Weight_ExpoDecay(
    double *image1,  // target
    double *image2,  // candidate
    double image1_mean,
    double image2_mean,
    double NODATA,
    int size,
    double ExpoDecay_mu,
    double ExpoDecay_sigma
)
{
    int counts = 0;
    double sum = 0.0;
    double w;
    for (size_t i = 0; i < size; i++)
    {
        if (isNODATA(*(image1 + i), NODATA) == 0)
        {
            counts += 1;
            w = exp(ExpoDecay_sigma * abs(*(image1 + i) - ExpoDecay_mu));
            sum += (*(image1 + i) - image1_mean) * (*(image2 + i) - image2_mean) * w;
        }
    }
    if (counts <= 1)
    {
        printf("NULL: an empty image is detected!\n");
        exit(1);
    }
    return (1 / ((double) counts - 1) * sum);
}



