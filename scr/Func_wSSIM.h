#ifndef Func_wSSIM
#define Func_wSSIM

double weightSSIM_Gaussian(
    double *image1,
    double *image2,
    double NODATA,
    int size,
    double *k,
    double *power
);

double mean_Weight_Gaussian(
    double *image,
    double NODATA,
    int size,
    double gaussian_mu,
    double gaussian_sigma
);

double SD_Weight_Gaussian(
    double *image,
    double image_mean,
    double NODATA,
    int size,
    double gaussian_mu,
    double gaussian_sigma
);

double CV_Weight_Gaussian(
    double *image1,  // target
    double *image2,  // candidate
    double image1_mean,
    double image2_mean,
    double NODATA,
    int size,
    double gaussian_mu,
    double gaussian_sigma
);

/************
 * Exponential decay weight function
 * *********/

double weightSSIM_ExpoDecay(
    double *image1,
    double *image2,
    double NODATA,
    int size,
    double *k,
    double *power
);

double mean_Weight_ExpoDecay(
    double *image,
    double NODATA,
    int size,
    double ExpoDecay_mu,
    double ExpoDecay_sigma
);

double SD_Weight_ExpoDecay(
    double *image,
    double image_mean,
    double NODATA,
    int size,
    double ExpoDecay_mu,
    double ExpoDecay_sigma
);

double CV_Weight_ExpoDecay(
    double *image1,  // target
    double *image2,  // candidate
    double image1_mean,
    double image2_mean,
    double NODATA,
    int size,
    double ExpoDecay_mu,
    double ExpoDecay_sigma
);


#endif

