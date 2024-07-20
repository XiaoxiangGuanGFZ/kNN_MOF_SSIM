#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "def_struct.h"
#include "Func_dataIO.h"
#include "Func_Initialize.h"
#include "Func_SSIM.h"
#include "Func_Fragments.h"
#include "Func_kNN.h"
#include "Func_Disaggregate.h"
#include "Func_Prepro.h"
#include "Func_Recursive.h"
#include "Func_Print.h"

/****** exit description *****
 * void exit(int status);
 * from <stdlib.h>
 *  The exit() function takes a single argument, status,
 * which is an integer value. This status code is
 * returned to the parent process or the operating system.
 * By convention, a status code of 0 usually indicates successful execution,
 * and any other non-zero value typically indicates
 * an error or abnormal termination.
 * --------
 * 0: successfuly execution
 * 1: file input or output error
 * 2: algorithm error
 *
 *****************************/

FILE *p_SSIM;
FILE *p_log;  // file pointer pointing to log file
int FLAG_LOG;

/***************************************
 * main function 
**************************************/
int main(int argc, char * argv[]) {
    /*
    int argc: the number of parameters of main() function;
    char *argv[]: pointer array
    */
    /* char fname[100] = "D:/kNN_MOF_cp/data/global_para.txt";
        this should be the only extern input for this program */
    time_t tm;  //datatype from <time.h>
    time(&tm);

    struct Para_global Para_df;     // define the global-parameter structure
    struct Para_global *p_gp;      // give a pointer to global_parameter structure
    p_gp = &Para_df;

    /******* import the global parameters ***********
    parameter from main() function, pointer array
    argv[0]: pointing to the first string from command line (the executable file)
    argv[1]: pointing to the second string (parameter): file path and name of global parameter file.
    */
    import_global(*(++argv), p_gp);
    FLAG_LOG = p_gp->FLAG_LOG;
    if (FLAG_LOG == 1)
    {
        if ((p_log = fopen(p_gp->FP_LOG, "a+")) == NULL)
        {
            printf("cannot create / open log file\n");
            exit(1);
        }
    }
    Print_gp(p_gp);
    /******* import circulation pattern series *********/
    
    static struct df_cp df_cps[MAXrow];
    int nrow_cp=0;  // the number of CP data columns: 4 (y, m, d, cp)
    if (strncmp(p_gp->T_CP, "TRUE", 4) == 0) {
        nrow_cp = import_df_cp(Para_df.FP_CP, df_cps);
        Print_cp(df_cps, nrow_cp);
    }
    if (strncmp(p_gp->MONTH, "TRUE", 4) == 0)
    {
        time(&tm);
        printf("------ Disaggregation conditioned on 12 months: %s", ctime(&tm));
        if (FLAG_LOG == 1)
        {
            fprintf(p_log, "------ Disaggregation conditioned on 12 months: %s", ctime(&tm));/* code */
        }
    }
    else if (strncmp(p_gp->SEASON, "TRUE", 4) == 0)
    {
        printf("------ Disaggregation conditioned on seasonality-summer and winter\n");
        if (FLAG_LOG == 1)
        {
            fprintf(p_log, "------ Disaggregation conditioned on seasonality-summer and winter\n");
        }
    }

    /****** import rainsite coor *******/
    // struct df_coor *p_coor;
    // int nrow_coor;
    // p_coor = (struct df_coor *)malloc(sizeof(struct df_coor) * p_gp->N_STATION * 2);
    // nrow_coor = import_df_coor(p_gp->FP_COOR, p_coor);
    // initialize_df_coor(p_gp, &p_coor, nrow_coor);
    // time(&tm); printf("------ Import rainsite coor (Done): %s", ctime(&tm)); 
    // fprintf(p_log, "------ Import rainsite coor (Done): %s", ctime(&tm));

    /****** import daily rainfall data (to be disaggregated) *******/
    
    static struct df_rr_d df_rr_daily[MAXrow];
    int nrow_rr_d;
    nrow_rr_d = import_dfrr_d(Para_df.FP_DAILY, Para_df.N_STATION, df_rr_daily);
    initialize_dfrr_d(p_gp, df_rr_daily, df_cps, nrow_rr_d, nrow_cp);
    Print_dly(df_rr_daily, p_gp, nrow_rr_d);

    /****** import hourly rainfall data (obs as fragments) *******/
    
    int ndays_h;
    static struct df_rr_h df_rr_hourly[MAXrow];
    ndays_h = import_dfrr_h(Para_df.FP_HOURLY, Para_df.N_STATION, df_rr_hourly);
    initialize_dfrr_h(p_gp, df_rr_hourly, df_cps, ndays_h, nrow_cp);
    Print_hly(df_rr_hourly, ndays_h);

    /****** rainfall data preprocessing: wet-dry status initialization *******/
    initialize_dfrr_wd(p_gp, df_rr_daily, df_rr_hourly, nrow_rr_d, ndays_h);
    /****** rainfall data preprocessing: normalization / standardization *******/
    // preprocessing of the data: none [0], normalization [1] or standardization [2]
    if (p_gp->PREPROCESS != 0)
    {
        if (p_gp->PREPROCESS == 1)
        {
            Normalize_rain(p_gp, df_rr_daily, df_rr_hourly, nrow_rr_d, ndays_h);
        }
        time(&tm);
        printf("------ Rainfall data preprocessing (Done): %s", ctime(&tm));
        if (FLAG_LOG == 1)
        {
            fprintf(p_log, "------ Rainfall data preprocessing (Done): %s", ctime(&tm));
        }
    }
    
    /****** Disaggregation: kNN_MOF_cp *******/
    if (p_gp->flag_SSIM == 1)  // if (strncmp(p_gp->FP_SSIM, "FALSE", 5) == 0)
    {
        if ((p_SSIM = fopen(p_gp->FP_SSIM, "w")) == NULL)
        {
            printf("Cannot create / open SSIM file: %s\n", p_gp->FP_SSIM);
            exit(1);
        }
        if (FLAG_LOG == 1)
        {
            fprintf(p_SSIM, "target,ID,index_Frag,SSIM,candidate\n");
        }
    }

    printf("------ Disaggregating: ... \n");
    kNN_MOF_SSIM_Recursive(
        df_rr_hourly,
        df_rr_daily,
        df_cps,
        p_gp,
        nrow_rr_d,
        ndays_h,
        nrow_cp);
    // kNN_MOF_SSIM(
    //     df_rr_hourly,
    //     df_rr_daily,
    //     df_cps,
    //     p_gp,
    //     p_coor,
    //     nrow_rr_d,
    //     ndays_h,
    //     nrow_cp);
    fclose(p_SSIM);
    time(&tm);
    printf("------ Disaggregation daily2hourly (Done): %s", ctime(&tm));
    if (FLAG_LOG == 1)
    {
        fprintf(p_log, "------ Disaggregation daily2hourly (Done): %s", ctime(&tm));
    }
    return 0; 
}
