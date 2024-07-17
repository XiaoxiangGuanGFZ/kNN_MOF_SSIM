
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "def_struct.h"
#include "Func_Initialize.h"
#include "Func_dataIO.h"
#include "Func_Print.h"

void Print_gp(
    struct Para_global *p_gp
)
{
    time_t tm;  //datatype from <time.h>
    time(&tm);
        printf("------ Global parameter import completed: %s", ctime(&tm));
    fprintf(p_log, "------ Global parameter import completed: %s", ctime(&tm));

    printf(
        "FP_DAILY: %s\nFP_HOULY: %s\nFP_CP:    %s\nFP_OUT:   %s\nFP_LOG:   %s\n",
        p_gp->FP_DAILY, p_gp->FP_HOURLY, p_gp->FP_CP, p_gp->FP_OUT, p_gp->FP_LOG
    );
    fprintf(p_log, "FP_DAILY: %s\nFP_HOULY: %s\nFP_CP:    %s\nFP_OUT:   %s\nFP_LOG:   %s\n",
        p_gp->FP_DAILY, p_gp->FP_HOURLY, p_gp->FP_CP, p_gp->FP_OUT, p_gp->FP_LOG);

    printf(
        "------ Disaggregation parameters: -----\nPREPROCESS: %d\nT_CP: %s\nN_STATION: %d\nCONTINUITY: %d\nWD: %d\nMONTH: %s\nSEASON: %s\n",
        p_gp->PREPROCESS, p_gp->T_CP, p_gp->N_STATION, p_gp->CONTINUITY, p_gp->WD, p_gp->MONTH, p_gp->SEASON
    );
    fprintf(
        p_log,
        "------ Disaggregation parameters: -----\nPREPROCESS: %d\nT_CP: %s\nN_STATION: %d\nCONTINUITY: %d\nWD: %d\nMONTH: %s\nSEASON: %s\n",
        p_gp->PREPROCESS, p_gp->T_CP, p_gp->N_STATION, p_gp->CONTINUITY, p_gp->WD, p_gp->MONTH, p_gp->SEASON
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

}

void Print_cp(
    struct df_cp *df_cps,
    int nrow_cp
)
{
    time_t tm; // datatype from <time.h>
    time(&tm);
    printf("------ Import CP data series (Done): %s", ctime(&tm));
    fprintf(p_log, "------ Import CP data series (Done): %s", ctime(&tm));

    printf("* number of CP data rows: %d\n", nrow_cp);
    fprintf(p_log, "* number of CP data rows: %d\n", nrow_cp);

    printf("* the first day: %d-%02d-%02d \n", df_cps[0].date.y, df_cps[0].date.m, df_cps[0].date.d);
    fprintf(p_log, "* the first day: %d-%02d-%02d \n", df_cps[0].date.y, df_cps[0].date.m, df_cps[0].date.d);

    printf("* the last day:  %d-%02d-%02d \n",
           df_cps[nrow_cp - 1].date.y, df_cps[nrow_cp - 1].date.m, df_cps[nrow_cp - 1].date.d);
    fprintf(p_log, "* the last day:  %d-%02d-%02d \n",
            df_cps[nrow_cp - 1].date.y, df_cps[nrow_cp - 1].date.m, df_cps[nrow_cp - 1].date.d);
}

void Print_dly(
    struct df_rr_d *df_dly,
    struct Para_global *p_gp,
    int nrow_rr_d
)
{
    time_t tm; // datatype from <time.h>
    time(&tm);
    printf("------ Import daily data (Done): %s", ctime(&tm)); fprintf(p_log, "------ Import daily data (Done): %s", ctime(&tm));
    
    printf("* the total rows: %d\n", nrow_rr_d); fprintf(p_log, "* the total rows: %d\n", nrow_rr_d);
    
    printf("* the first day: %d-%02d-%02d\n", df_dly[0].date.y,df_dly[0].date.m,df_dly[0].date.d);
    fprintf(p_log, "* the first day: %d-%02d-%02d\n", df_dly[0].date.y,df_dly[0].date.m,df_dly[0].date.d);

    printf(
        "* the last day:  %d-%02d-%02d\n", 
        df_dly[nrow_rr_d-1].date.y,df_dly[nrow_rr_d-1].date.m,df_dly[nrow_rr_d-1].date.d
    );
    fprintf(
        p_log,
        "* the last day:  %d-%02d-%02d\n", 
        df_dly[nrow_rr_d-1].date.y,df_dly[nrow_rr_d-1].date.m,df_dly[nrow_rr_d-1].date.d
    );
    printf("* the total classes:  %d\n", p_gp->CLASS_N);
    fprintf(p_log, "* the total classes:  %d\n", p_gp->CLASS_N);
    view_class_rrd(df_dly, nrow_rr_d);
}

void Print_hly(
    struct df_rr_h *df_hly,
    int ndays_h
)
{
    time_t tm; // datatype from <time.h>
    time(&tm);
    printf("------ Import hourly data (Done): %s", ctime(&tm)); fprintf(p_log, "------ Import hourly data (Done): %s", ctime(&tm));
    
    printf("* total hourly obs days: %d\n", ndays_h); fprintf(p_log, "* total hourly obs days: %d\n", ndays_h);
    
    printf("* the first day: %d-%02d-%02d\n", df_hly[0].date.y, df_hly[0].date.m, df_hly[0].date.d);
    fprintf(p_log, "* the first day: %d-%02d-%02d\n", df_hly[0].date.y, df_hly[0].date.m, df_hly[0].date.d);
    
    printf(
        "* the last day:  %d-%02d-%02d\n", 
        df_hly[ndays_h-1].date.y, df_hly[ndays_h-1].date.m, df_hly[ndays_h-1].date.d
    );
    fprintf(
        p_log,
        "* the last day:  %d-%02d-%02d\n", 
        df_hly[ndays_h-1].date.y, df_hly[ndays_h-1].date.m, df_hly[ndays_h-1].date.d
    );
    view_class_rrh(df_hly, ndays_h);
}

