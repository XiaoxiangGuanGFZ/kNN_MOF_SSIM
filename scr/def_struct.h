#ifndef STRUCT_H
#define STRUCT_H

#define MAXCHAR 10000  // able to accomodate up to 3000 sites simultaneously
#define MAXrow 100000  // almost 270 years long ts
#define MAXcps 20
#define FloatZero 0.005

/******
 * the following define the structures
*/
struct Date {
    int y;
    int m;
    int d;
};
struct df_rr_d
{
    /* data
     * data frame for the daily step precipitation,
     * p_rr points to a double-type array, 
     *      with the size equal to the number of stations 
     */
    struct Date date;    
    double *p_rr;
    double *p_rr_pre;
    int wd;  // wet or dry
    int cp;  // the circulation pattern type / class
    int SM;  // the seasonality
    int class; // combination of co and SM
};

struct df_coor
{
    int id;
    double lon;
    double lat;
    int *neighbors;
} ;

struct df_rr_h
{
    /* data
     * dataframe for the hourly step precipitation, 
     * rr_h: pointer array; 
     *      24 double-type pointers; 
     *      each points to an array of hourly precipitation (all rain sites)
     * rr_d: double-type pointer;
     *      pointing to an array of daily precipitation (all rain site) aggregated from rr_h
     */
    struct Date date;    
    double (*rr_h)[24];
    double *rr_d;
    double *rr_d_pre;
    int wd;  // wet or dry
    int cp;
    int SM;
    int class;
};

struct df_cp
{
    /* 
     * circulation pattern classficiation series
     * each day with a CP class
     */
    struct Date date;    
    int cp;
};

struct Para_global
    {
        /* global parameters */
        char FP_COOR[200];      // file path of rain site coordinates
        char FP_DAILY[200];     // file path of daily precipitation data (to be disaggregated)
        char FP_CP[200];        // file path of circulation pattern (CP) classification data series
        char FP_HOURLY[200];    // file path of hourly precipitation data (as fragments)
        char FP_OUT[200];       // file path of output(hourly) precipitation from disaggregation
        char FP_LOG[200];       // file path of log file
        char FP_SSIM[200];      // file path of the output SSIM file

        int PREPROCESS;         // preprocess the data by normalization or standardization
        int N_STATION;          // number of stations (rain sites)
        
        char T_CP[10];          // toggle (flag), whether the CP is considered in the algorithm
        char MONTH[10];         // toggle (flag), conditioned on month: 12 months
        char SEASON[10];        // toggle (flag), whether the seasonality is considered in the algorithm
        int SUMMER_FROM;        // the beginning month of summer
        int SUMMER_TO;          // the end month of summer

        int CONTINUITY;         // continuity day
        int WD;                 // the flexibility level of wet-dry status in candidates filtering
        int CLASS_N;            // total categories the series is classified into

        /*************
         * SSIM parameters
         * ********/
        double k[3];            // 3 parameters in SSIM
        double power[3];        // 3 paras in SSIM
        double NODATA;          // nodata value
        int flag_SSIM;          // flag, whether to write SSIM 

        int RUN;                // simulation runs 
    };


#endif
