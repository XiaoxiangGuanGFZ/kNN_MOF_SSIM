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
#include "Func_recursive.h"

void kNN_MOF_SSIM_Recursive(
    struct df_rr_h *p_rrh,
    struct df_rr_d *p_rrd,
    struct df_cp *p_cp,
    struct Para_global *p_gp,
    struct df_coor *p_coor,
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

    FILE *p_FP_OUT;
    if ((p_FP_OUT = fopen(p_gp->FP_OUT, "w")) == NULL)
    {
        printf("Program terminated: cannot create or open output file\n");
        exit(1);
    }
    for (i = 0; i < nrow_rr_d; i++)
    {
        // iterate each target day
        Initialize_output(&df_rr_h_out, p_gp, p_rrd, i);

        Toggle_wd = 0;                                                   // initialize with 0 (non-rainy)
        Toggle_wd = Toggle_WD(p_gp->N_STATION, (p_rrd + i)->p_rr);
        if (Toggle_wd == 0)
        {
            // this is a non-rainy day; all 0.0
            n_can = -1;
            // for (size_t t = 0; t < p_gp->RUN; t++)
            // {
            //     /* write the disaggregation output */
            //     Write_df_rr_h(&df_rr_h_out, p_gp, p_FP_OUT, t + 1);
            // }
            Write_df_rr_h(&df_rr_h_out, p_gp, p_FP_OUT, 1);
        }
        else
        {
            // Toggle_wd == 1; this is a rainy day; we will disaggregate it.
            int index = 0;
            /********
             * flexible in wet-dry status filtering
             * n_can: the number of candidates after wet-dry status filtering
             * ********/
            n_can = Toggle_CONTINUITY(p_rrh, p_rrd + i, p_gp, ndays_h, pool_cans);
            /**********
             * filtering: 
             * - class: cp type, 12 months, or season (winter or summer)
             * - continuity: the wet-dry status before and after the target day
             * ********/
            index = Filter_WD_Class(
                    p_rrh, p_rrd, p_gp,
                    i, (p_rrd + i)->class,
                    ndays_h, nrow_rr_d,
                    n_can, pool_cans);
            // index:  the number of candidates after class filtering
            if (index == 0)
            {
                printf("No candidates!\n");
                exit(1);
            }
            n_can = index; // number of candidates (pool size)
            /***************
             * here candidate pool derived 
             * ************/
            // ---------------------------------------------------

            // for (size_t t = 0; t < p_gp->RUN; t++)
            // {
            //     seed_random();
            //     // sample from candidate pool and assign the fragments simultaneously
            //     kNN_SSIM_sampling_recursive(p_rrd, p_rrh, p_gp, &df_rr_h_out,
            //                                 i, pool_cans, n_can, skip);
            //     /* write the disaggregation output */
            //     Write_df_rr_h(&df_rr_h_out, p_gp, p_FP_OUT, t + 1);
            // }
            // sample from candidate pool and assign the fragments simultaneously
            kNN_SSIM_sampling_recursive(p_rrd, p_rrh, p_gp, &df_rr_h_out,
                                        i, pool_cans, n_can, skip);
            /* write the disaggregation output */
            Write_df_rr_h(&df_rr_h_out, p_gp, p_FP_OUT, 1);
        }

        printf("%d-%02d-%02d: Done!\n", (p_rrd + i)->date.y, (p_rrd + i)->date.m, (p_rrd + i)->date.d);
        // free(df_rr_h_out.rr_h); // free the memory allocated for disaggregated hourly output
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
    int skip
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
    double w_image[5] = {0.08333333, 0.1666667, 0.5, 0.1666667, 0.08333333}; // CONTUNITY == 5
    if (skip == 0)
    {
        // CONTUNITY == 1
        w_image[0] = 1.0;
    }
    else if (skip == 1)
    {
        // CONTUNITY == 3
        w_image[0] = 0.1666667;
        w_image[1] = 0.6666667;
        w_image[2] = 0.1666667;
    }
    else if (skip > 5)
    {
        printf("Currently CONTUNITY > 5 is not possible!\n");
        exit(1);
    }
    int i, j, s; // iteration variable
    
    double *SSIM;    // the distance between target day and candidate days
    double SSIM_temp;
    SSIM = (double *)malloc(n_can * sizeof(double));

    /** compute mean-SSIM between target and candidate images **/
    for (i = 0; i < n_can; i++)
    {
        *(SSIM + i) = 0.0;
        for (s = 0 - skip; s < 1 + skip; s++)
        {
            if (Toggle_WD(p_gp->N_STATION, (p_rrd + index_target + s)->p_rr) == 0)
            {
                SSIM_temp = w_image[s + skip] * 1.0;
            }
            else
            {
                SSIM_temp = w_image[s + skip] * meanSSIM(
                                                    (p_rrd + index_target + s)->p_rr_pre,
                                                    (p_rrh + pool_cans[i] + s)->rr_d_pre,
                                                    p_gp->NODATA,
                                                    p_gp->N_STATION,
                                                    p_gp->k,
                                                    p_gp->power);
            }
            *(SSIM + i) += SSIM_temp;
        }
    }
    int order = 1; // larger SSIM, heavier weight
    int run = 1;
    int index_fragment;
    kNN_sampling(SSIM, pool_cans, order, n_can, run, &index_fragment);
    // printf("n_can: %d, fragment: %d\n", n_can, index_fragment);

    Fragment_assign_recursive(p_rrh, p_out, p_rrd, p_gp, index_target, index_fragment);
    if (Toggle_WD(p_gp->N_STATION, p_out->rr_d) == 1)
    {
        kNN_SSIM_sampling_recursive(p_rrd, p_rrh, p_gp, p_out,
                                    index_target, pool_cans, n_can, skip);
    }

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
    p_out->rr_d = (p_rrd + index_target)->p_rr;
    /*******************
     * initialize the output (hourly) data,
     * preset as 0.0 
     * *****************/
    for (int j = 0; j < p_gp->N_STATION; j++)
    {
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
        if (p_out->rr_d[j] > 0.0)
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
                (p_rrd + index_target)->p_rr[j] = 0.0;
                (p_rrd + index_target)->p_rr_pre[j] = 0.0;
            }
        }
    }
}
