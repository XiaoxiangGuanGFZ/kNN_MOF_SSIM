

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "def_struct.h"
#include "Func_Fragments.h"

int Toggle_WD(
    int N_STATION,
    double *p_rr_d)
{
    /***********
     * rainy day (wet, WD == 1) or non rainy day (dry, WD == 0)
     * - wet: as long as there is a wet site
     * - dry: not a single wet site
     ***********/
    int WD = 0;
    for (size_t i = 0; i < N_STATION; i++)
    {
        if (*(p_rr_d + i) > 0.0)
        {
            WD = 1;
            break;
        }
    }
    return WD;
}

int Toggle_CONTINUITY(
    struct df_rr_h *p_rrh,
    struct df_rr_d *p_rrd,
    struct Para_global *p_gp,
    int ndays_h,
    int pool_cans[])
{
    /*****************
     * Description:
     *      wet-dry status continuity matching check
     * Parameters:
     *      p_rrh: pointing to df_rr_hourly struct array (all the hourly rr observations);
     *      p_rrd: pointing to target day (to be disaggregated);
     *      p_gp: pointing to global parameter struct;
     *      ndays_h: number of days in hourly rr data;
     *      pool_cans: the pool of candidate index;
     * Output:
     *      - the number of candidates (pool size)
     *      - bring back the pool_cans array
     * ***************/
    int skip, match;                          // match: whether wet-dry status match; 1: match; 0: not match
    int n_cans = 0;                           // number of candidates fullfilling the CONTINUITY criteria (output)
    skip = (int)((p_gp->CONTINUITY - 1) / 2); // p_gp->CONTINUITY == 1: skip=0; p_gp->CONTINUITY == 3: skip=1
    int WD;
    /*****
     * WD:
     * - 1: flexiable
     * - 0: strict, perfect match in wet-dry status
     * - -1: no requirement, as long as the candidate day is also wet
     * ***/
    if (WD == 1)
    {
        for (int k = skip; k < ndays_h - skip; k++)
        {
            // iterate each day in hourly observations
            match = 1;
            for (int s = 0; s < p_gp->N_STATION; s++)
            {
                /* flexible wet-dry status matching*/
                if (p_rrd->p_rr[s] > FloatZero && (p_rrh + k)->rr_d[s] <= FloatZero)
                {
                    match = 0;
                    break;
                }
            }

            if (match == 1)
            {
                *(pool_cans + n_cans) = k;
                n_cans++;
            }
        }
    }
    else if (WD == 0)
    {
        // strict; wet == wet or dry == dry for each site
        for (int k = skip; k < ndays_h - skip; k++)
        {
            // iterate each day in hourly observations
            match = 1;
            for (int s = 0; s < p_gp->N_STATION; s++)
            {
                if (
                    (p_rrd->p_rr[s] > FloatZero && (p_rrh + k)->rr_d[s] <= FloatZero) || 
                    (p_rrd->p_rr[s] <= FloatZero && (p_rrh + k)->rr_d[s] > FloatZero)
                    )
                {
                    match = 0;
                    break;
                }
            }

            if (match == 1)
            {
                *(pool_cans + n_cans) = k;
                n_cans++;
            }
        }
    }
    else if (WD == -1)
    {
        /********
         * as long as it is a wet day; don't care the wet-dry match 
         * ****/
        for (int k = skip; k < ndays_h - skip; k++)
        {
            if (Toggle_WD(p_gp->N_STATION, (p_rrh + k)->rr_d) == 1)
            {
                *(pool_cans + n_cans) = k;
                n_cans++;
            }
        }
    }
    return n_cans;
}

int Filter_WD_multisite(
    struct df_rr_h *p_rrh,
    double *p_rr_t,
    int N_STATION,
    int n_can,
    int pool_cans[],
    int pool_cans_final[],
    int WD
)
{
    /*****
     * WD:
     * - 1: flexiable
     * - 0: strict, perfect match in wet-dry status
     * - -1: no requirement, as long as the candidate day is also wet
     * ***/
    int match = 0;
    int index = 0;
    if (WD == 1)
    {
        for (int k = 0; k < n_can; k++)
        {
            // iterate each day in hourly observations
            match = 1;
            for (int s = 0; s < N_STATION; s++)
            {
                /* flexible wet-dry status matching*/
                if (*(p_rr_t + s) > 0 && (p_rrh + pool_cans[k])->rr_d[s] <= 0)
                {
                    match = 0;
                    break;
                }
            }

            if (match == 1)
            {
                *(pool_cans_final + index) = pool_cans[k];
                index++;
            }
        }
    }
    else if (WD == 0)
    {
        // strict; wet == wet or dry == dry for each site
        for (int k = 0; k < n_can; k++)
        {
            // iterate each day in hourly observations
            match = 1;
            for (int s = 0; s < N_STATION; s++)
            {
                if (
                    (*(p_rr_t + s) > 0 && (p_rrh + pool_cans[k])->rr_d[s] <= 0) || 
                    (*(p_rr_t + s) <= 0 && (p_rrh + pool_cans[k])->rr_d[s] > 0)
                    )
                {
                    match = 0;
                    break;
                }
            }

            if (match == 1)
            {
                *(pool_cans_final + index) = pool_cans[k];
                index++;
            }
        }
    }
    else if (WD == -1)
    {
        /********
         * as long as it is a wet day; don't care the wet-dry match 
         * ****/
        index = n_can;
        for (int k = 0; k < n_can; k++)
        {
            *(pool_cans_final + k) = pool_cans[k];
        }
    }
    return index;
}


int Filter_WD_Class(
    struct df_rr_h *p_rrh,
    struct df_rr_d *p_rrd,
    int id_target,
    int ndays_h,
    int pool_cans[]
)
{
    /**************************
     * filter the candidate pools based on the following criteria:
     * - class: either cp type or seasonality(12 months or 2 seasons)
     * - continuity: the wet-dry status before and after the target and candidate day
     * PS: in SSIM calculation, empty map could bring error or bias
     * ***********************/
    int index = 0;
    int CLASS_candidate, CLASS_target;
    int wd_target, wd_candidate;
    CLASS_target = (p_rrd + id_target)->class;
    wd_target = (p_rrd + id_target)->wd;
    
    for (size_t i = 0; i < ndays_h; i++)
    {
        /*********
         * criteria:
         * - class: cp type and seasonality
         * - the days before and after the target day exist, if CONTUNITY > 1
         * - wet-dry status of the days before and after the target day should be identical to those of the candidate
         * ******/
        CLASS_candidate = (p_rrh + i)->class; // the class of the candidate day
        wd_candidate = (p_rrh + i)->wd;
        if (CLASS_candidate == CLASS_target && wd_candidate == wd_target) 
        {
            pool_cans[index] = i;
            index += 1;
        }
    }
    return(index);  // the number of candidates after filtering 
}

void Fragment_assign(
    struct df_rr_h *p_rrh,
    struct df_rr_h *p_out,
    struct Para_global *p_gp,
    struct df_coor *p_coor,
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
        {
            if ((p_rrh + fragment)->rr_d[j] <= 0.0)
            {
                /********
                 * the rain site in the target day is wet while in candidate day it is dry,
                 * then no direct fragments can be borrowed to disaggregate.
                 * **/

                // for (h = 0; h < 24; h++)
                // {
                //     // just distribute evenly
                //     p_out->rr_h[j][h] = p_out->rr_d[j] * 1 / 24;
                // }

                /*********
                 * borrow fragments from nearest rainy (wet) neighbour
                 */
                int id;
                int index = 0;
                int wd = 0;
                while (wd == 0 && index < p_gp->N_STATION - 1)
                {
                    // p_gp->N_STATION - 1: number of neighbours
                    id = *((p_coor + j)->neighbors + index);
                    if ((p_rrh + fragment)->rr_d[id] > 0.0)
                    {
                        // the neighbour is wet
                        wd = 1;
                    }
                    else
                    {
                        index += 1;
                    }
                }
                for (h = 0; h < 24; h++)
                {
                    p_out->rr_h[j][h] = p_out->rr_d[j] * (p_rrh + fragment)->rr_h[id][h] / (p_rrh + fragment)->rr_d[id];
                }
            }
            else
            {
                for (h = 0; h < 24; h++)
                {
                    p_out->rr_h[j][h] = p_out->rr_d[j] * (p_rrh + fragment)->rr_h[j][h] / (p_rrh + fragment)->rr_d[j];
                }
            }
        }
        else
        {
            // no rain at the station j
            for (h = 0; h < 24; h++)
            {
                p_out->rr_h[j][h] = 0.0;
            }
        }
    }
}

void View_df_h(
    struct df_rr_h *p_out,
    int N_STATION
)
{
    printf("daily: \n");
    for (size_t i = 0; i < N_STATION; i++)
    {
        printf("%.1f,", *(p_out->rr_d + i));
    }
    printf("\nhourly: \n");

    for (size_t h = 0; h < 24; h++)
    {
        for (size_t i = 0; i < N_STATION; i++)
        {
            printf("%.1f,", p_out->rr_h[i][h]);
        }
        printf("\n");
    }
}