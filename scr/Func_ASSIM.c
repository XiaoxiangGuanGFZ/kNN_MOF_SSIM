
#include <stdio.h>
#include <math.h>
#include "Func_ASSIM.h"
#include "Func_SSIM.h"


double ASSIM(
    double *image1,
    double *image2,
    double NODATA,
    int size,
    double *power,
    double small_thd
)
{
    /**********
     * amplitude and structural similarity index measure
     * ********/
    int counts = 0;
    double image1_mean, image2_mean;
    double image1_sd, image2_sd; 
    double image_cov;
    image1_mean = mean(image1, NODATA, size);
    image2_mean = mean(image2, NODATA, size);
    image1_sd = StandardDeviation(image1, image1_mean, NODATA, size);
    image2_sd = StandardDeviation(image2, image2_mean, NODATA, size);
    image_cov = covariance(image1, image2, image1_mean, image2_mean, NODATA, size);

    double SSIM_l, SSIM_c, SSIM_s, ASSIM;

    // similarity in amplitude
    if (image1_mean + image2_mean < small_thd)
    {
        SSIM_l = 1;
    } else {
        SSIM_l = (2 * image1_mean * image2_mean) / (pow(image1_mean, 2) + pow(image2_mean, 2));
    }
    
    // similarity in variance
    if (pow(image1_sd, 2) + pow(image2_sd, 2) < pow(small_thd, 2))
    {
        SSIM_c = 1;
    } else {
        SSIM_c = (2 * image1_sd * image2_sd) / (pow(image1_sd, 2) + pow(image2_sd, 2));
    }
    
    // similarity in point-wise linear correspondence
    if (image_cov < 0)  // a negative covariance
    {
        SSIM_s = 0;
    } else if (image1_sd < small_thd && image2_sd < small_thd)
    {
        SSIM_s = 1;
    } else if (image_cov >= 0)
    {
        SSIM_s = (image_cov) / (image1_sd * image2_sd);
    } 
    
    ASSIM = pow(SSIM_l, *(power + 0)) * pow(SSIM_c, *(power + 1)) * pow(SSIM_s, *(power + 2));
    // printf("SSIM_l:%f, SSIM_c:%f, SSIM_s:%f, SSIM:%f\n", SSIM_l, SSIM_c, SSIM_s, ASSIM);
    return ASSIM;
}



