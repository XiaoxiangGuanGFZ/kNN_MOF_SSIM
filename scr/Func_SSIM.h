#ifndef FUNC_SSIM
#define FUNC_SSIM

double meanSSIM(
    double *image1,
    double *image2,
    double NODATA,
    double L,
    int size,
    double *k,
    double *power
);

double mean(
    double *image,
    double NODATA,
    int size
);

double StandardDeviation(
    double *image,
    double image_mean,
    double NODATA,
    int size
);

double covariance(
    double *image1,
    double *image2,
    double image1_mean,
    double image2_mean,
    double NODATA,
    int size
);

int isNODATA(
    double x,
    double NODATA
);

#endif