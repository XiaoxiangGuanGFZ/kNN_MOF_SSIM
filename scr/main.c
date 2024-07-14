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
#include "Func_recursive.h"

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
*/

FILE *p_SSIM;
FILE *p_log;  // file pointer pointing to log file

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
    
    if ((p_log=fopen(p_gp->FP_LOG, "a+")) == NULL) {
        printf("cannot create / open log file\n");
        exit(1);
    }
    printf("------ Global parameter import completed: %s", ctime(&tm));
    fprintf(p_log, "------ Global parameter import completed: %s", ctime(&tm));

    printf(
        "FP_DAILY: %s\nFP_HOULY: %s\nFP_CP:    %s\nFP_OUT:   %s\nFP_LOG:   %s\n",
        p_gp->FP_DAILY, p_gp->FP_HOURLY, p_gp->FP_CP, p_gp->FP_OUT, p_gp->FP_LOG
    );
    fprintf(p_log, "FP_DAILY: %s\nFP_HOULY: %s\nFP_CP:    %s\nFP_OUT:   %s\nFP_LOG:   %s\n",
        p_gp->FP_DAILY, p_gp->FP_HOURLY, p_gp->FP_CP, p_gp->FP_OUT, p_gp->FP_LOG);

    printf(
        "------ Disaggregation parameters: -----\nT_CP: %s\nN_STATION: %d\nCONTINUITY: %d\nWD: %d\nSEASON: %s\n",
        p_gp->T_CP, p_gp->N_STATION, p_gp->CONTINUITY, p_gp->WD, p_gp->SEASON
    );
    fprintf(
        p_log,
        "------ Disaggregation parameters: -----\nT_CP: %s\nN_STATION: %d\nCONTINUITY: %d\nWD: %d\nSEASON: %s\n",
        p_gp->T_CP, p_gp->N_STATION, p_gp->CONTINUITY, p_gp->WD, p_gp->SEASON
    );
    if (strncmp(p_gp->SEASON, "TRUE", 4) == 0)
    {
        printf("SUMMER: %d-%d\n", p_gp->SUMMER_FROM, p_gp->SUMMER_TO);
        fprintf(p_log,"SUMMER: %d-%d\n", p_gp->SUMMER_FROM, p_gp->SUMMER_TO);
    }
    printf("SSIM_K: %f,%f,%f\nSSIM_power: %f,%f,%f\nNODATA: %f\n",
           p_gp->k[0], p_gp->k[1], p_gp->k[2], p_gp->power[0], p_gp->power[1], p_gp->power[2],
           p_gp->NODATA);
    fprintf(p_log, "SSIM_K: %f,%f,%f\nSSIM_power: %f,%f,%f\nNODATA: %f\n",
            p_gp->k[0], p_gp->k[1], p_gp->k[2], p_gp->power[0], p_gp->power[1], p_gp->power[2],
            p_gp->NODATA);

    /******* import circulation pattern series *********/
    
    static struct df_cp df_cps[MAXrow];
    int nrow_cp=0;  // the number of CP data columns: 4 (y, m, d, cp)
    if (strncmp(p_gp->T_CP, "TRUE", 4) == 0) {
        nrow_cp = import_df_cp(Para_df.FP_CP, df_cps);
        time(&tm);
        printf("------ Import CP data series (Done): %s", ctime(&tm)); 
        fprintf(p_log, "------ Import CP data series (Done): %s", ctime(&tm));

        printf("* number of CP data rows: %d\n", nrow_cp); 
        fprintf(p_log, "* number of CP data rows: %d\n", nrow_cp);

        printf("* the first day: %d-%02d-%02d \n", df_cps[0].date.y, df_cps[0].date.m, df_cps[0].date.d);
        fprintf(p_log, "* the first day: %d-%02d-%02d \n", df_cps[0].date.y, df_cps[0].date.m, df_cps[0].date.d);
        
        printf("* the last day:  %d-%02d-%02d \n", 
            df_cps[nrow_cp-1].date.y, df_cps[nrow_cp-1].date.m, df_cps[nrow_cp-1].date.d
        );
        fprintf(p_log, "* the last day:  %d-%02d-%02d \n", 
            df_cps[nrow_cp-1].date.y, df_cps[nrow_cp-1].date.m, df_cps[nrow_cp-1].date.d
        );
    } else {
        time(&tm);
        printf("------ Disaggregation conditioned only on seasonality (12 months): %s", ctime(&tm));
        fprintf(p_log, "------ Disaggregation conditioned only on seasonality (12 months): %s", ctime(&tm));
    }

    /****** import rainsite coor *******/
    struct df_coor *p_coor;
    int nrow_coor;
    p_coor = (struct df_coor *)malloc(sizeof(struct df_coor) * p_gp->N_STATION * 2);
    nrow_coor = import_df_coor(p_gp->FP_COOR, p_coor);
    initialize_df_coor(p_gp, &p_coor, nrow_coor);
    time(&tm); printf("------ Import rainsite coor (Done): %s", ctime(&tm)); 
    fprintf(p_log, "------ Import rainsite coor (Done): %s", ctime(&tm));

    /****** import daily rainfall data (to be disaggregated) *******/
    
    static struct df_rr_d df_rr_daily[MAXrow];
    int nrow_rr_d;
    nrow_rr_d = import_dfrr_d(Para_df.FP_DAILY, Para_df.N_STATION, df_rr_daily);
    initialize_dfrr_d(p_gp, df_rr_daily, df_cps, nrow_rr_d, nrow_cp);

    time(&tm);
    printf("------ Import daily rainfall data (Done): %s", ctime(&tm)); fprintf(p_log, "------ Import daily rainfall data (Done): %s", ctime(&tm));
    
    printf("* the total rows: %d\n", nrow_rr_d); fprintf(p_log, "* the total rows: %d\n", nrow_rr_d);
    
    printf("* the first day: %d-%02d-%02d\n", df_rr_daily[0].date.y,df_rr_daily[0].date.m,df_rr_daily[0].date.d);
    fprintf(p_log, "* the first day: %d-%02d-%02d\n", df_rr_daily[0].date.y,df_rr_daily[0].date.m,df_rr_daily[0].date.d);

    printf(
        "* the last day:  %d-%02d-%02d\n", 
        df_rr_daily[nrow_rr_d-1].date.y,df_rr_daily[nrow_rr_d-1].date.m,df_rr_daily[nrow_rr_d-1].date.d
    );
    fprintf(
        p_log,
        "* the last day:  %d-%02d-%02d\n", 
        df_rr_daily[nrow_rr_d-1].date.y,df_rr_daily[nrow_rr_d-1].date.m,df_rr_daily[nrow_rr_d-1].date.d
    );

    view_class_rrd(df_rr_daily, nrow_rr_d);

    /****** import hourly rainfall data (obs as fragments) *******/
    
    int ndays_h;
    static struct df_rr_h df_rr_hourly[MAXrow];
    ndays_h = import_dfrr_h(Para_df.FP_HOURLY, Para_df.N_STATION, df_rr_hourly);
    initialize_dfrr_h(p_gp, df_rr_hourly, df_cps, ndays_h, nrow_cp);
    
    time(&tm);
    printf("------ Import hourly rainfall data (Done): %s", ctime(&tm)); fprintf(p_log, "------ Import hourly rainfall data (Done): %s", ctime(&tm));
    
    printf("* total hourly obs days: %d\n", ndays_h); fprintf(p_log, "* total hourly obs days: %d\n", ndays_h);
    
    printf("* the first day: %d-%02d-%02d\n", df_rr_hourly[0].date.y, df_rr_hourly[0].date.m, df_rr_hourly[0].date.d);
    fprintf(p_log, "* the first day: %d-%02d-%02d\n", df_rr_hourly[0].date.y, df_rr_hourly[0].date.m, df_rr_hourly[0].date.d);
    
    printf(
        "* the last day:  %d-%02d-%02d\n", 
        df_rr_hourly[ndays_h-1].date.y, df_rr_hourly[ndays_h-1].date.m, df_rr_hourly[ndays_h-1].date.d
    );
    fprintf(
        p_log,
        "* the last day:  %d-%02d-%02d\n", 
        df_rr_hourly[ndays_h-1].date.y, df_rr_hourly[ndays_h-1].date.m, df_rr_hourly[ndays_h-1].date.d
    );
    view_class_rrh(df_rr_hourly, ndays_h);

    /****** rainfall data preprocessing: wet-dry status initialization *******/
    initialize_dfrr_wd(p_gp, df_rr_daily, df_rr_hourly, nrow_rr_d, ndays_h);
    /****** rainfall data preprocessing: normalization / standardization *******/
    Normalize_rain(p_gp, df_rr_daily, df_rr_hourly, nrow_rr_d, ndays_h);
    
    time(&tm);
    printf("------ Rainfall data preprocessing (Done): %s", ctime(&tm));
    fprintf(p_log, "------ Rainfall data preprocessing (Done): %s", ctime(&tm));
    
    /****** Disaggregation: kNN_MOF_cp *******/
    if (p_gp->flag_SSIM == 1)
    {
        if ((p_SSIM = fopen(p_gp->FP_SSIM, "w")) == NULL)
        {
            printf("Cannot create / open SSIM file: %s\n", p_gp->FP_SSIM);
            exit(1);
        }
        fprintf(p_SSIM, "target,ID,index_Frag,SSIM,candidate\n");
    }

    printf("------ Disaggregating: ... \n");
    kNN_MOF_SSIM_Recursive(
        df_rr_hourly,
        df_rr_daily,
        df_cps,
        p_gp,
        p_coor,
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
    fprintf(p_log, "------ Disaggregation daily2hourly (Done): %s", ctime(&tm));
    return 0; 
}
