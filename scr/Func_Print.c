
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "def_struct.h"
#include "Func_Initialize.h"
#include "Func_dataIO.h"
#include "Func_Print.h"

void Print_gp(
    struct Para_global *p_gp)
{
    time_t tm; // datatype from <time.h>
    time(&tm);
    printf("\n------ Global parameter import completed: %s", ctime(&tm));
    printf(
        "%-10s: %s\n%-10s: %s\n%-10s: %s\n%-10s: %s\n%-10s: %s\n%-10s: %s\n",
        "FP_DAILY", p_gp->FP_DAILY,
        "FP_HOURLY", p_gp->FP_HOURLY,
        "FP_CP", p_gp->FP_CP,
        "FP_OUT", p_gp->FP_OUT,
        "FP_LOG", p_gp->FP_LOG,
        "FP_SSIM", p_gp->FP_SSIM);
    printf(
        "------ Disaggregation parameters: \n%-10s: %d\n%-10s: %s\n%-10s: %s\n%-10s: %d\n%-10s: %d\n%-10s: %d\n%-10s: %s\n%-10s: %s\n",
        "PREPROCESS", p_gp->PREPROCESS,
        "SIMILARITY", p_gp->SIMILARITY,
        "T_CP", p_gp->T_CP,
        "N_STATION", p_gp->N_STATION,
        "CONTINUITY", p_gp->CONTINUITY,
        "WD", p_gp->WD,
        "MONTH", p_gp->MONTH,
        "SEASON", p_gp->SEASON);

    if (FLAG_LOG == 1)
    {
        fprintf(p_log, "\n------ Global parameter import completed: %s", ctime(&tm));
        fprintf(p_log,
                "%-10s: %s\n%-10s: %s\n%-10s: %s\n%-10s: %s\n%-10s: %s\n%-10s: %s\n",
                "FP_DAILY", p_gp->FP_DAILY,
                "FP_HOURLY", p_gp->FP_HOURLY,
                "FP_CP", p_gp->FP_CP,
                "FP_OUT", p_gp->FP_OUT,
                "FP_LOG", p_gp->FP_LOG,
                "FP_SSIM", p_gp->FP_SSIM);
        fprintf(
            p_log,
            "------ Disaggregation parameters: -----\n%-10s: %d\n%-10s: %s\n%-10s: %s\n%-10s: %d\n%-10s: %d\n%-10s: %d\n%-10s: %s\n%-10s: %s\n",
            "PREPROCESS", p_gp->PREPROCESS,
            "SIMILARITY", p_gp->SIMILARITY,
            "T_CP", p_gp->T_CP,
            "N_STATION", p_gp->N_STATION,
            "CONTINUITY", p_gp->CONTINUITY,
            "WD", p_gp->WD,
            "MONTH", p_gp->MONTH,
            "SEASON", p_gp->SEASON);
    }

    if (strncmp(p_gp->SEASON, "TRUE", 4) == 0)
    {
        printf("%-10s: %d-%d\n", "SUMMER", p_gp->SUMMER_FROM, p_gp->SUMMER_TO);
        if (FLAG_LOG == 1)
        {
            fprintf(p_log, "%-10s: %d-%d\n", "SUMMER", p_gp->SUMMER_FROM, p_gp->SUMMER_TO);
        }
    }
    printf("%-10s: %f,%f,%f\n%-10s: %f,%f,%f\n%-10s: %f\n",
           "SSIM_K",
           p_gp->k[0], p_gp->k[1], p_gp->k[2],
           "SSIM_power",
           p_gp->power[0], p_gp->power[1], p_gp->power[2],
           "NODATA", p_gp->NODATA);
    if (FLAG_LOG == 1)
    {
        fprintf(p_log,
                "%-10s: %f,%f,%f\n%-10s: %f,%f,%f\n%-10s: %f\n",
                "SSIM_K",
                p_gp->k[0], p_gp->k[1], p_gp->k[2],
                "SSIM_power",
                p_gp->power[0], p_gp->power[1], p_gp->power[2],
                "NODATA", p_gp->NODATA);
    }
}

void Print_cp(
    struct df_cp *df_cps,
    int nrow_cp)
{
    time_t tm; // datatype from <time.h>
    time(&tm);
    printf("------ Import CP data series (Done): %s", ctime(&tm));
    printf("* number of CP data rows: %d\n", nrow_cp);
    printf("* the first day: %d-%02d-%02d \n", df_cps[0].date.y, df_cps[0].date.m, df_cps[0].date.d);
    printf("* the last day:  %d-%02d-%02d \n",
           df_cps[nrow_cp - 1].date.y, df_cps[nrow_cp - 1].date.m, df_cps[nrow_cp - 1].date.d);
    
    if (FLAG_LOG == 1)
    {
        fprintf(p_log, "------ Import CP data series (Done): %s", ctime(&tm));
        fprintf(p_log, "* number of CP data rows: %d\n", nrow_cp);
        fprintf(p_log, "* the first day: %d-%02d-%02d \n", df_cps[0].date.y, df_cps[0].date.m, df_cps[0].date.d);
        fprintf(p_log, "* the last day:  %d-%02d-%02d \n",
            df_cps[nrow_cp - 1].date.y, df_cps[nrow_cp - 1].date.m, df_cps[nrow_cp - 1].date.d);
    }
    
}

void Print_dly(
    struct df_rr_d *df_dly,
    struct Para_global *p_gp,
    int nrow_rr_d)
{
    time_t tm; // datatype from <time.h>
    time(&tm);
    printf("------ Import daily data (Done): %s", ctime(&tm));
    printf("* the total rows: %d\n", nrow_rr_d);
    printf("* the first day: %d-%02d-%02d\n", df_dly[0].date.y, df_dly[0].date.m, df_dly[0].date.d);
    printf("* the last day:  %d-%02d-%02d\n",
           df_dly[nrow_rr_d - 1].date.y, df_dly[nrow_rr_d - 1].date.m, df_dly[nrow_rr_d - 1].date.d);
    printf("* the total classes:  %d\n", p_gp->CLASS_N);

    if (FLAG_LOG == 1)
    {
        fprintf(p_log, "------ Import daily data (Done): %s", ctime(&tm));
        fprintf(p_log, "* the total rows: %d\n", nrow_rr_d);
        fprintf(p_log, "* the first day: %d-%02d-%02d\n", df_dly[0].date.y, df_dly[0].date.m, df_dly[0].date.d);
        fprintf(p_log, "* the last day:  %d-%02d-%02d\n",
                df_dly[nrow_rr_d - 1].date.y, df_dly[nrow_rr_d - 1].date.m, df_dly[nrow_rr_d - 1].date.d);
        fprintf(p_log, "* the total classes:  %d\n", p_gp->CLASS_N);
    }
    
    view_class_rrd(df_dly, nrow_rr_d);
}

void Print_hly(
    struct df_rr_h *df_hly,
    int ndays_h)
{
    time_t tm; // datatype from <time.h>
    time(&tm);
    printf("------ Import hourly data (Done): %s", ctime(&tm));
    printf("* total hourly obs days: %d\n", ndays_h);
    printf("* the first day: %d-%02d-%02d\n", df_hly[0].date.y, df_hly[0].date.m, df_hly[0].date.d);
    printf("* the last day:  %d-%02d-%02d\n",
           df_hly[ndays_h - 1].date.y, df_hly[ndays_h - 1].date.m, df_hly[ndays_h - 1].date.d);

    if (FLAG_LOG == 1)
    {
        fprintf(p_log, "------ Import hourly data (Done): %s", ctime(&tm));
        fprintf(p_log, "* total hourly obs days: %d\n", ndays_h);
        fprintf(p_log, "* the first day: %d-%02d-%02d\n", df_hly[0].date.y, df_hly[0].date.m, df_hly[0].date.d);
        fprintf(p_log,
                "* the last day:  %d-%02d-%02d\n",
                df_hly[ndays_h - 1].date.y, df_hly[ndays_h - 1].date.m, df_hly[ndays_h - 1].date.d);
    }

    view_class_rrh(df_hly, ndays_h);
}
