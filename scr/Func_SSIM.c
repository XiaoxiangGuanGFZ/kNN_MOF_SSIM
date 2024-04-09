#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Func_SSIM.h"


double meanSSIM(
    double *image1,
    double *image2,
    double NODATA,
    double L,
    int size,
    double *k,
    double *power
)
{
    int counts = 0;
    double image1_mean, image2_mean;
    double image1_sd, image2_sd; 
    double image_cov;
    image1_mean = mean(image1, NODATA, size);
    image2_mean = mean(image2, NODATA, size);
    image1_sd = StandardDeviation(image1, image1_mean, NODATA, size);
    image2_sd = StandardDeviation(image2, image2_mean, NODATA, size);
    image_cov = covariance(image1, image2, image1_mean, image2_mean, NODATA, size);

    double SSIM_l, SSIM_c, SSIM_s, SSIM;
    double C[3];
    for (size_t i = 0; i < 3; i++)
    {
        C[i] = pow(*(k + i) * L, 2);
    }
    
    SSIM_l = (2 * image1_mean * image2_mean + C[0]) / (pow(image1_mean, 2) + pow(image2_mean, 2) + C[0]);
    SSIM_c = (2 * image1_sd * image2_sd + C[1]) / (pow(image1_sd, 2) + pow(image2_sd, 2) + C[1]);
    SSIM_s = (image_cov + C[2]) / (image1_sd * image2_sd + C[2]);
    SSIM = pow(SSIM_l, *(power + 0)) * pow(SSIM_c, *(power + 1)) * pow(SSIM_s, *(power + 2));
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
        if (isNODATA(*(image+i), NODATA) == 0)
        {
            counts += 1;
            sum += *(image+i);
        }
    }
    if (counts == 0)
    {
        printf("NULL: an empty image is detected!\n");
        exit(1);
    }
    return (sum / counts);
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
        if (isNODATA(*(image+i), NODATA) == 0)
        {
            counts += 1;
            square_sum += pow(*(image+i) - image_mean, 2);
        }
    }
    if (counts <= 1)
    {
        printf("NULL: an empty image is detected!\n");
        exit(1);
    }
    return pow(1 / (counts - 1) * square_sum, 0.5);
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
        if (isNODATA(*(image1+i), NODATA) == 0)
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
    return 1 / (counts - 1) * sum;
}

int isNODATA(
    double x,
    double NODATA
)
{
    double delta = 0.01;
    if (x >= (NODATA - delta) && x <= (NODATA + delta))
    {
        return 1;
    } else {
        return 0;
    }
}

