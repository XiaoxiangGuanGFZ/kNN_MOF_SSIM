#ifndef STRUCT_H
#define STRUCT_H

#define MAXCHAR 10000  // able to accomodate up to 3000 sites simultaneously
#define MAXrow 100000  // almost 270 years long ts
#define MAXcps 20
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
    int cp;
    int season;
    int class;
};
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
    int cp;
    int season;
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
        char FP_DAILY[150];     // file path of daily precipitation data (to be disaggregated)
        char FP_CP[150];        // file path of circulation pattern (CP) classification data series
        char FP_HOURLY[150];    // file path of hourly precipitation data (as fragments)
        char FP_OUT[150];       // file path of output(hourly) precipitation from disaggregation
        char FP_LOG[150];       // file path of log file
        int N_STATION;          // number of stations (rain sites)
        char T_CP[10];          // toggle (flag), whether the CP is considered in the algorithm
        char SEASON[10];        // toggle (flag), whether the seasonality is considered in the algorithm
        int SUMMER_FROM;        // the beginning month of summer
        int SUMMER_TO;          // the end month of summer
        int CONTINUITY;         // continuity day
        int WD;                 // the flexibility level of wet-dry status in candidates filtering
        int CLASS_N;            // total categories the series is classified into

        double k[3];            // 3 parameters in SSIM
        double power[3];        // 3 paras in SSIM
        double NODATA;          // nodata value
        int RUN;                // simulation runs 
    };


#endif
