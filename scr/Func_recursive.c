/*
 * SUMMARY:      Func_recursive.c
 * USAGE:        main algorithm of method-of-fragments for temporal disaggregation
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Apr-2024
 * DESCRIPTION:  the method-of-fragments conditioned on :
 *               - circulation patterns (classification)
 *               - seasonality (summer or winter)
 *               - the similarity is represented by SSIM (structural Similarity Index Measure)
 *               - kNN is used to consider the uncertainty or variability
 * DESCRIP-END.
 * FUNCTIONS:    kNN_MOF_SSIM_Recursive(); 
 *
 * COMMENTS:
 *
 * REFERENCEs:
 */

/*******************************************************************************
 * VARIABLEs:
 * 
 *****/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include "def_struct.h"
#include "Func_SSIM.h"
#include "Func_dataIO.h"
#include "Func_kNN.h"
#include "Func_Fragments.h"
#include "Func_Recursive.h"

void kNN_MOF_SSIM_Recursive(
    struct df_rr_h *p_rrh,
    struct df_rr_d *p_rrd,
    struct df_cp *p_cp,
    struct Para_global *p_gp,
    int nrow_rr_d,
    int ndays_h,
    int nrow_cp)
{
    /*******************
     * Description:
     *  algorithm: k-nearest neighbouring sampling, method-of-fragments, circulation patterns and (or) seasonality
     *  scale: daily2hourly, multiple rain gauges simultaneously
     * Parameters:
     *  nrow_rr_d: the number of rows in daily rr data file
     *  ndays_h: the number of observations of hourly rr data
     *  nrow_cp: the number of rows in cp series
     * *****************/
    int i, j, h, k, s, Toggle_wd;
    int class_t, class_c;

    struct df_rr_h df_rr_h_out; // this is a struct variable, not a struct array;
    df_rr_h_out.rr_h = calloc(p_gp->N_STATION, sizeof(double) * 24); // allocate memory (stack);
    df_rr_h_out.rr_d = calloc(p_gp->N_STATION, sizeof(double));
    df_rr_h_out.rr_d_pre = calloc(p_gp->N_STATION, sizeof(double));

    /************
     * CONTINUITY and skip
     * - p_gp->CONTINUITY: 1, skip = 0;
     * - p_gp->CONTINUITY: 3, skip = 1;
     * - p_gp->CONTINUITY: 5, skip = 2;
     * *************/
    int skip = 0;
    skip = (int)((p_gp->CONTINUITY - 1) / 2);
    int pool_cans[MAXrow]; // the index of the candidates (a pool); the size is sufficient
    int n_can;             // the number of candidates after all conditioning (cp and seasonality)
    int fragment;          // the index of df_rr_h structure with the final chosed fragments
    int WD;
    
    FILE *p_FP_OUT;
    if ((p_FP_OUT = fopen(p_gp->FP_OUT, "w")) == NULL)
    {
        printf("Program terminated: cannot create or open output file\n");
        exit(1);
    }
    for (i = 0; i < nrow_rr_d; i++) // iterate each target day
    {
        WD = p_gp->WD;
        Toggle_wd = 0;                                                   // initialize with 0 (non-rainy)
        Toggle_wd = Toggle_WD(p_gp->N_STATION, (p_rrd + i)->p_rr);
        if (Toggle_wd == 0)
        {
            // this is a non-rainy day; all 0.0
            n_can = -1;
            Initialize_output(&df_rr_h_out, p_gp, p_rrd, i); // View_df_h(&df_rr_h_out, p_gp->N_STATION);
            for (size_t t = 0; t < p_gp->RUN; t++)
            {
                /* write the disaggregation output */
                Write_df_rr_h(&df_rr_h_out, p_gp, p_FP_OUT, t + 1);
            }
        }
        else
        {
            /**********
             * filtering: 
             * - class: cp type, 12 months, or season (winter or summer)
             * - n_can: the number of candidates after wet-dry status filtering
             * ********/
            n_can = Filter_WD_Class(p_rrh, p_rrd, i, ndays_h, pool_cans);
            // sample from candidate pool and assign the fragments simultaneously
            for (int t = 0; t < p_gp->RUN; t++)
            {
                int depth = 0;
                seed_random();
                Initialize_output(&df_rr_h_out, p_gp, p_rrd, i); // View_df_h(&df_rr_h_out, p_gp->N_STATION);
                kNN_SSIM_sampling_recursive(p_rrd, p_rrh, p_gp, &df_rr_h_out, i, pool_cans, n_can, &WD, &depth);
                Write_df_rr_h(&df_rr_h_out, p_gp, p_FP_OUT, t + 1); /* write the disaggregation output */
            }
        }
        printf("%d-%02d-%02d: Done!\n", (p_rrd + i)->date.y, (p_rrd + i)->date.m, (p_rrd + i)->date.d);
    }
    fclose(p_FP_OUT);
}

void kNN_SSIM_sampling_recursive(
    struct df_rr_d *p_rrd,
    struct df_rr_h *p_rrh,
    struct Para_global *p_gp,
    struct df_rr_h *p_out,
    int index_target,
    int pool_cans[],
    int n_can,
    int *WD,
    int *depth
)
{
/**************
     * Description:
     *      - compute the SSIM index of rr between target and candidate days
     *      - sort the distance in the decreasing order
     *      - select the sqrt(n_can) largest SSIM
     *      - weights defined based on SSIM: higher SSIM, heavier weight
     *      - sample one candidate
     * Parameters:
     *      p_rrd: the daily rainfall st (structure pointer)
     *      p_rrh: pointing to the hourly rr obs structure array
     *      p_gp: pointing to global parameter structure
     *      index_target: the index of target day to be disaggregated
     *      pool_cans: the index pool of candidats
     *      n_can: the number (or size) fo candidates pool
     *      skip: due to the consideration of days before and after the target day,
     *              the first and last several days should be disaggregated by assuming CONTUNITY == 1
     * Output:
     *      none;
     *      modify the struct df_rr_h *p_out directly
     * ***********/
    // printf("depth: %d\n", *depth);
    int *pool_cans_final;
    int n_can_final;
    pool_cans_final = (int *)malloc(sizeof(int) * n_can);
    if (*depth >= 5) {*WD = 1;}
    *depth += 1;
    n_can_final = Filter_WD_multisite(p_rrh, p_out->rr_d, p_gp->N_STATION, n_can, pool_cans, pool_cans_final, *WD);
    if (n_can_final == 0)
    {
        printf("No candidae after multi-site wet-dry status filtering!\n");
        exit(0);
    }
    
    // printf("n_can_final: %d\n", n_can_final);
    int i; 
    double *SSIM; 
    SSIM = (double *)malloc(n_can_final * sizeof(double));
    /** compute mean-SSIM between target and candidate images **/
    if (p_gp->PREPROCESS == 1 && strcmp(p_gp->SIMILARITY, "SSIM") == 0)
    {
        for (i = 0; i < n_can_final; i++)
        {
            *(SSIM + i) = meanSSIM(p_out->rr_d_pre, (p_rrh + pool_cans_final[i])->rr_d_pre, p_gp->NODATA, p_gp->N_STATION, p_gp->k, p_gp->power);
        }
    }
    else if (p_gp->PREPROCESS == 0 && strcmp(p_gp->SIMILARITY, "SSIM") == 0)
    {
        for (i = 0; i < n_can_final; i++)
        {
            *(SSIM + i) = meanSSIM(p_out->rr_d, (p_rrh + pool_cans_final[i])->rr_d, p_gp->NODATA, p_gp->N_STATION, p_gp->k, p_gp->power);
        }
    }
    else if (p_gp->PREPROCESS == 1 && strcmp(p_gp->SIMILARITY, "Manhattan") == 0)
    {
        for (i = 0; i < n_can_final; i++)
        {
            *(SSIM + i) = Manhattan_distance(p_out->rr_d_pre, (p_rrh + pool_cans_final[i])->rr_d_pre, p_gp->N_STATION);
        }
    }
    else if (p_gp->PREPROCESS == 0 && strcmp(p_gp->SIMILARITY, "Manhattan") == 0)
    {
        for (i = 0; i < n_can_final; i++)
        {
            *(SSIM + i) = Manhattan_distance(p_out->rr_d, (p_rrh + pool_cans_final[i])->rr_d, p_gp->N_STATION);
        }
    }

    int order = 1; // 1: larger SSIM, heavier weight; 0: larger distance, less weight
    if (strcmp(p_gp->SIMILARITY, "Manhattan") == 0)
    {
        order = 0;
    }
    
    int run = 1;   // sample one candidate each time
    int index_fragment;
    kNN_sampling(SSIM, pool_cans_final, order, n_can_final, run, &index_fragment);
    
    // printf("n_can: %d, fragment: %d\n", n_can_final, index_fragment);
    Fragment_assign_recursive(p_rrh, p_out, p_rrd, p_gp, index_target, index_fragment);
    if (Toggle_WD(p_gp->N_STATION, p_out->rr_d) == 1)
    {
        kNN_SSIM_sampling_recursive(p_rrd, p_rrh, p_gp, p_out, index_target, pool_cans, n_can, WD, depth);
    }

    free(pool_cans_final);
    free(SSIM);
}

void Initialize_output(
    struct df_rr_h *p_out,
    struct Para_global *p_gp,
    struct df_rr_d *p_rrd,
    int index_target
)
{
    p_out->date = (p_rrd + index_target)->date;
    for (int j = 0; j < p_gp->N_STATION; j++)
    {
        *(p_out->rr_d + j) = *((p_rrd + index_target)->p_rr + j);
        if (p_gp->PREPROCESS != 0)
        {
            *(p_out->rr_d_pre + j) = *((p_rrd + index_target)->p_rr_pre + j);
        }
        /*******************
         * initialize the output (hourly) data,
         * preset as 0.0
         * *****************/
        for (int h = 0; h < 24; h++)
        {
            p_out->rr_h[j][h] = 0.0;
        }
    }
}

void Fragment_assign_recursive(
    struct df_rr_h *p_rrh,
    struct df_rr_h *p_out,
    struct df_rr_d *p_rrd,
    struct Para_global *p_gp,
    int index_target,
    int fragment)
{
    /**********
     * Description:
     *      disaggregate the target day rainfall into hourly scale based on the selected fragments
     * Parameters:
     *      p_rrh: pointing to the hourly obs rr structure array
     *      p_out: pointing to the disaggregated hourly rr results struct (to output)
     *      p_gp: global parameters struct
     *      fragment: the index of p_rrh struct after filtering and resampling
     * Output:
     *      p_out
     * *******/
    int j, h;
    for (j = 0; j < p_gp->N_STATION; j++)
    {
        if (p_out->rr_d[j] > 0)
        {   // wet site
            if ((p_rrh + fragment)->rr_d[j] > 0.0){
                // the same site in candidate day is also wet, then disaggregate it
                for (h = 0; h < 24; h++)
                {
                    p_out->rr_h[j][h] = p_out->rr_d[j] * (p_rrh + fragment)->rr_h[j][h] / (p_rrh + fragment)->rr_d[j];
                }
                /**************
                 * after disaggregating this site, the following
                 * should be updated
                 * **********/
                p_out->rr_d[j] = 0.0;
                if (p_gp->PREPROCESS != 0)
                {
                    p_out->rr_d_pre[j] = 0.0;
                }
            }
        }
    }
}

