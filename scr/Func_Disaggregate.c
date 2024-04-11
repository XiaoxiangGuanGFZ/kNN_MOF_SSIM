/*
 * SUMMARY:      Func_Disaggregate.c
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
 * FUNCTIONS:    kNN_MOF_SSIM(); Toggle_WD(); kNN_SSIM_sampling();
 *               get_random(); weight_cdf_sample(); Fragment_assign();
 * 
 * COMMENTS:
 * 
 * REFERENCEs:
 * Xiaoxiang Guan, Katrin Nissen, Viet Dung Nguyen, Bruno Merz, Benjamin Winter, Sergiy Vorogushyn. 
 *      Multisite temporal rainfall disaggregation using methods of fragments conditioned on circulation patterns. 
 *      Journal of Hydrology, 621, 129640. doi: https://doi.org/10.1016/j.jhydrol.2023.129640
 */

/*******************************************************************************
 * VARIABLEs:
 * 
*****/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "def_struct.h"
#include "Func_SSIM.h"
#include "Func_Disaggregate.h"
#include "Func_dataIO.h"

void kNN_MOF_SSIM(
    struct df_rr_h *p_rrh,
    struct df_rr_d *p_rrd,
    struct df_cp *p_cp,
    struct Para_global *p_gp,
    int nrow_rr_d,
    int ndays_h,
    int nrow_cp
) {
    /*******************
     * Description:
     *  algorithm: k-nearest neighbouring sampling, method-of-fragments, circulation patterns and (or) seasonality
     *  scale: daily2hourly, multiple rain gauges simultaneously
     * Parameters:
     *  nrow_rr_d: the number of rows in daily rr data file
     *  ndays_h: the number of observations of hourly rr data
     *  nrow_cp: the number of rows in cp series
     * *****************/
    int i, j, h, k, Toggle_wd;
    int toggle_cp;
    int class_t, class_c;
    int skip=0;
    struct df_rr_h df_rr_h_out; // this is a struct variable, not a struct array;
    // struct df_rr_h df_rr_h_candidates[100];
    // p_gp->CONTINUITY: 1, skip = 0;
    // p_gp->CONTINUITY: 3, skip = 1;
    skip = (p_gp->CONTINUITY - 1) / 2;
    int pool_cans[MAXrow];  // the index of the candidates (a pool); the size is sufficient 
    int n_can; //the number of candidates after all conditioning (cp and seasonality)
    int fragment; // the index of df_rr_h structure with the final chosed fragments
    
    FILE *p_FP_OUT;
    if ((p_FP_OUT=fopen(p_gp->FP_OUT, "w")) == NULL) {
        printf("Program terminated: cannot create or open output file\n");
        exit(1);
    }
    for (i=skip; i < nrow_rr_d-skip; i++) { //i=skip
        // iterate each (possible) target day
        df_rr_h_out.date = (p_rrd + i)->date;
        df_rr_h_out.rr_d = (p_rrd + i)->p_rr; // is this valid?; address transfer
        df_rr_h_out.rr_h = calloc(p_gp->N_STATION, sizeof(double) * 24);  // allocate memory (stack);
        Toggle_wd = 0;  // initialize with 0 (non-rainy)
        for (j=0; j < p_gp->N_STATION; j++) {
            if (*((p_rrd + i)->p_rr + j) > 0.0) {
                // any gauge with rr > 0.0
                Toggle_wd = 1;
                break;
            }
        }
        // printf("Targetday: %d-%d-%d: %d\n", 
        //     (p_rrd + i)->date.y, (p_rrd + i)->date.m, (p_rrd + i)->date.d, Toggle_wd
        // );
        if (Toggle_wd == 0) {
            // this is a non-rainy day; all 0.0
            n_can = -1;
            for (j=0; j<p_gp->N_STATION; j++){
                for (h=0; h<24; h++) {
                    df_rr_h_out.rr_h[j][h] = 0.0;
                }
            }
        } else {
            // Toggle_wd == 1;
            // this is a rainy day; we will disaggregate it.
            n_can = 0;
            class_t = (p_rrd + i)->class;
            for (j = 0; j < ndays_h; j++)
            {
                class_c = (p_rrh + j)->class; // the class of the candidate day
                if (class_c == class_t && Toggle_WD(p_gp->N_STATION, (p_rrh + j)->rr_d) == 1)
                {
                    pool_cans[n_can] = j;
                    n_can += 1;
                }
            }
            fragment = kNN_SSIM_sampling(p_rrd + i, p_rrh, p_gp, pool_cans, n_can);
            /*assign the sampled fragments to target day (disaggregation)*/
            Fragment_assign(p_rrh, &df_rr_h_out, p_gp, fragment);
        }
        /* write the disaggregation output */
        Write_df_rr_h(&df_rr_h_out, p_gp, p_FP_OUT);
        printf("%d-%02d-%02d: Done!\n", (p_rrd+i)->date.y, (p_rrd+i)->date.m, (p_rrd+i)->date.d);
        
        free(df_rr_h_out.rr_h);  // free the memory allocated for disaggregated hourly output
    }
    fclose(p_FP_OUT);
}

int Toggle_WD(
    int N_STATION,
    double *p_rr_d
)
{
    /***********
     * rainy day (wet, WD == 1) or non rainy day (dry, WD == 0)
    */
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



int kNN_SSIM_sampling(
    struct df_rr_d *p_rrd,
    struct df_rr_h *p_rrh,
    struct Para_global *p_gp,
    int pool_cans[],
    int n_can
){
    /**************
     * Description:
     *      - compute the SSIM index of rr between target and candidate days
     *      - sort the distance in the decreasing order
     *      - select the sqrt(n_can) largest SSIM
     *      - weights defined based on SSIM: higher SSIM, heavier weight
     *      - sample one candidate 
     * Parameters:
     *      p_rrd: the target day (structure pointer)
     *      p_rrh: pointing to the hourly rr obs structure array
     *      p_gp: pointing to global parameter structure
     *      pool_cans: the index pool of candidats
     *      n_can: the number (or size) fo candidates pool
     * Output:
     *      return a sampled index (fragments source)
     * ***********/
    int i, j;  // iteration variable
    int temp_c;  // temporary variable during sorting 
    double temp_d;
    double rd = 0.0;  // a random decimal value between 0.0 and 1.0

    // double distance[MAXrow]; 
    double *SSIM;  // the distance between target day and candidate days
    SSIM = (double *)malloc(n_can * sizeof(double));
    int size_pool; // the k in kNN
    int index_out; // the output of this function: the sampled fragment from candidates pool
    /** compute mean-SSIM between target and candidate images **/
    for (i = 0; i < n_can; i++)
    {
        *(SSIM + i) = -1;
        *(SSIM + i) = meanSSIM(
            p_rrd->p_rr,
            (p_rrh + pool_cans[i])->rr_d,
            p_gp->NODATA,
            p_gp->N_STATION,
            p_gp->k,
            p_gp->power);
        // printf("candidate index: %d, distance: %.2f\n", pool_cans[i], *(distance+i));
    }
    // sort the SSIM in the decreasing order
    for (i = 0; i < n_can - 1; i++)
    {
        for (j = i + 1; j < n_can; j++)
        {
            if (SSIM[i] < SSIM[j])
            {
                temp_c = pool_cans[i];
                pool_cans[i] = pool_cans[j];
                pool_cans[j] = temp_c;
                temp_d = SSIM[i];
                SSIM[i] = SSIM[j];
                SSIM[j] = temp_d;
            }
        }
    }
    /*
    printf("--------------after sorting---------\n");
    for (i=0; i<n_can;i++) {
        printf("candidate index: %d, distance: %.2f\n", pool_cans[i], *(distance+i));
    }
    */
    if (SSIM[0] > 1.0) {
        // the closest candidate with the distance of 0.0, then we skip the weighted sampling.
        index_out = pool_cans[0];
    } else {
        /***
         * the size of candidate pool in kNN algorithm
         *      the range of size_pool:
         *      [2, n_can]
        */
        size_pool = (int)sqrt(n_can) + 1; 
        /***
         * compute the weights for kNN sampling
         *      the weight is defined based on SSIM; higher SSIM, heavier weight
         * dynamic memory allocation for the double array - weights
         * */
        double *weights;
        weights = malloc(size_pool * sizeof(double)); // a double array with the size of size_pool
        double w_sum = 0.0; 
        for (i=0; i<size_pool; i++){
            *(weights+i) = SSIM[i] + 1;
            w_sum += SSIM[i] + 1;
            fprintf(p_SSIM, "%d-%02d-%02d,", p_rrd->date.y, p_rrd->date.m, p_rrd->date.d);
            fprintf(p_SSIM, "%d,%d,%f,", i, pool_cans[i], SSIM[i]);
            fprintf(p_SSIM, "%d-%02d-%02d\n", (p_rrh + pool_cans[i])->date.y, (p_rrh + pool_cans[i])->date.m, (p_rrh + pool_cans[i])->date.d);
        }
        for (i=0; i<size_pool; i++){
            *(weights+i) /= w_sum; // reassignment
        }
        /* compute the empirical cdf for weights (vector) */
        double *weights_cdf;
        weights_cdf = malloc(size_pool * sizeof(double));
        *(weights_cdf + 0) = weights[0];  // initialization
        for (i=1; i<size_pool; i++){
            *(weights_cdf + i) = *(weights_cdf + i-1) + weights[i];
        }
        /* generate a random number, then select the fragments index*/
        index_out = weight_cdf_sample(size_pool, pool_cans, weights_cdf);
        // for (j = 0; j < p_gp->N_STATION; j++)
        // {
        //     if (p_rrd->p_rr[j] > 0.0 && (p_rrh + index_out)->rr_d[j] <= 0.0)
        //     {
        //     }
        // }
        free(weights);
        free(weights_cdf);
    }
    free(SSIM);
    return index_out;
}


double get_random() { return ((double)rand() / (double)RAND_MAX); }
int weight_cdf_sample(
    int size_pool,
    int pool_cans[],
    double *weights_cdf    
) {
    int i;
    double rd = 0.0;  // a random decimal value between 0.0 and 1.0
    int index_out; // the output of this function: the sampled fragment from candidates pool

    // srand(time(NULL)); // randomize seed
    rd = get_random(); // call the function to get a different value of n every time
    if (rd <= weights_cdf[0])
    {
        index_out = pool_cans[0];
    }
    else
    {
        for (i = 1; i < size_pool; i++)
        {
            if (rd <= weights_cdf[i] && rd > weights_cdf[i - 1])
            {
                index_out = pool_cans[i];
                break;
            }
        }
    }
    return index_out;
}

void Fragment_assign(
    struct df_rr_h *p_rrh,
    struct df_rr_h *p_out,
    struct Para_global *p_gp,
    int fragment
){
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
                for (h = 0; h < 24; h++)
                {
                    p_out->rr_h[j][h] = p_out->rr_d[j] * 1 / 24;
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

