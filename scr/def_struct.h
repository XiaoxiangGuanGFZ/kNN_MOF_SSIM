#ifndef STRUCT_H
#define STRUCT_H

#define MAXCHAR 10000  // able to accomodate up to 3000 sites simultaneously
#define MAXrow 100000  // almost 270 years long ts

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
        int CONTINUITY;         // continuity day
        int WD;                 // the flexibility level of wet-dry status in candidates filtering
    };


#endif




/* ---- function declaration ---- */
// data import functions
void import_global(char fname[], struct Para_global *p_gp);  // function declaration
void removeLeadingSpaces(char *str);
int import_df_cp(
        char fname[],
        struct df_cp *p_df_cp
    );
int import_dfrr_d(
        char FP_daily[], 
        int N_STATION,
        struct df_rr_d *p_rr_d
    );  // declare
int import_dfrr_h(
        char FP_hourly[], 
        int N_STATION,
        struct df_rr_h *p_rr_h
    );

void Write_df_rr_h(
    struct df_rr_h *p_out,
    struct Para_global *p_gp,
    FILE *p_FP_OUT);

// kNN_MOF_cp algorithm functions
int Toggle_CONTINUITY(
        struct df_rr_h *p_rrh,
        struct df_rr_d *p_rrd,
        struct Para_global *p_gp,
        int ndays_h,
        int pool_cans[],
        int WD
    );

int Toogle_CP(
    struct Date date,
    struct df_cp *p_cp,
    int nrow_cp);

int kNN_sampling(
    struct df_rr_d *p_rrd,
    struct df_rr_h *p_rrh,
    struct Para_global *p_gp,
    int pool_cans[],
    int n_can);

void Fragment_assign(
    struct df_rr_h *p_rrh,
    struct df_rr_h *p_out,
    struct Para_global *p_gp,
    int fragment);

double get_random(); // declare
int weight_cdf_sample(
    int size_pool,
    int pool_cans[],
    double *weights_cdf    
);

void kNN_MOF(
        struct df_rr_h *p_rrh,
        struct df_rr_d *p_rrd,
        struct df_cp *p_cp,
        struct Para_global *p_gp,
        int nrow_rr_d,
        int ndays_h,
        int nrow_cp
    );

