#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "def_struct.h"

void kNN_MOF(
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
    int skip=0;
    struct df_rr_h df_rr_h_out; // this is a struct variable, not a struct array;
    // struct df_rr_h df_rr_h_candidates[100];
    // p_gp->CONTINUITY: 1, skip = 0;
    // p_gp->CONTINUITY: 3, skip = 1;
    skip = (p_gp->CONTINUITY - 1) / 2;
    int pool_cans[MAXrow];  // the index of the candidates (a pool); the size is sufficient 
    int n_cans_c=0;  // the number of candidates after continuity filtering
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
        n_cans_c = 0;
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
            n_cans_c = Toggle_CONTINUITY(
                p_rrh,
                p_rrd + i,  // pointing to the target day struct
                p_gp,
                ndays_h,
                pool_cans,
                p_gp->WD  // 
                /****
                 * WD == 0, strict; 
                 * wet-dry status vectors match with each other 
                 *      (candidate and target day) 
                 *      at multiple rain gauges perfectly.
                 * typically, WD == 1, flexible in wet-dry status filtering 
                 */
            );
            // printf("number of candidates after continuity filtering (n_cans_c): %d\n", n_cans_c);
            // printf("WD-0: ncan: %d\n", n_cans_c);
            if (n_cans_c < 2 && p_gp->WD == 0) {
                /***
                 * when the number of candidate days 
                 *      after strict (WD == 0) wet-dry status filter 
                 *      is less than 2.
                 * */ 
                n_cans_c = Toggle_CONTINUITY(
                            p_rrh,
                            p_rrd + i,
                            p_gp,
                            ndays_h,
                            pool_cans,
                            1 
                        );
                // printf("WD-1: ncan: %d\n", n_cans_c);
            }
            if (n_cans_c == 0) {
                printf("the target day %d-%02d-%02d has no matching hourly candidate!\n",
                       (p_rrd + i)->date.y, (p_rrd + i)->date.m, (p_rrd + i)->date.d);
                printf("Programm terminated!");
                exit(2);
            } else if (n_cans_c == 1) {
                /**
                 * only one candidate fits here after wet-dry status filtering 
                 *      then we just skip other filtering conditions, like
                 *      CP or seasonality.
                */
                fragment = pool_cans[0];
            } else {
                // n_cans_c > 1;
                // candidates filtering based on CP and seasonality
                n_can=0;
                for (int t = 0; t < n_cans_c; t++) {
                    toggle_cp = 0; // initialize the toggle_cp for every iteration
                    if (strncmp(p_gp->T_CP, "TRUE", 4) == 0) {
                        // disaggregation conditioned on circulation pattern classification
                        if (
                            Toogle_CP( (p_rrh + pool_cans[t])->date, p_cp, nrow_cp ) == Toogle_CP( (p_rrd + i)->date, p_cp, nrow_cp )
                        ) {
                            // only continue when target and candidate day share the same cp class.
                            if (strncmp(p_gp->SEASON, "TRUE", 4) == 0) {
                                // seasonality: summer and winter
                                if (
                                    ((p_rrh + pool_cans[t])->date.m >= 5 && (p_rrh + pool_cans[t])->date.m <= 10) == 
                                    ((p_rrd + i)->date.m >= 5 && (p_rrd + i)->date.m <= 10) 
                                ) {
                                    toggle_cp = 1;
                                }
                            } else {
                                // only cp, without seasonality
                                toggle_cp = 1;
                            }
                        } 
                    } else {
                        /***
                         * p_gp->T_CP != "TRUE": 
                         *      the disaggregation conditioned only on seasonality (12 months)
                        */
                        if (
                            (p_rrh + pool_cans[t])->date.m == (p_rrd + i)->date.m
                        ) {
                            toggle_cp = 1;
                        }
                    }
                    if (toggle_cp == 1) {
                        // this candidate selected into (final) pool
                        pool_cans[n_can] = pool_cans[t];  // rewrite pool_cans[] array; t is always >= n_can
                        n_can++;
                    }
                }
                // printf("number of candidates after all filtering (n_can): %d\n", n_can);
                /* printf("Candidates: \n");
                for (int ii = 0; ii < n_can; ii++) {
                    printf("%d\n", pool_cans[ii]);
                } */ 
                /******** filtering is done: *********/
                if (n_can == 1) {
                    /***
                     * after candidates filtering, only one left.
                    */
                    fragment = pool_cans[0];
                } else {
                    // n_can == 0 or n_can > 1
                    if (n_can == 0) {
                        /***
                         * after cp and seasonality filtering, 
                         *      there is not candidates any more 
                         *      (toggle_cp always equals to 0).
                         *      pool_cans remains unchanged.
                         * Then we just discard the cp and seasonality filtering
                         *      in order to make the disaggregation possible.
                        */
                        n_can = n_cans_c;
                    }
                    /* kNN sampling, based on the qualified candidates pool */
                    fragment = kNN_sampling(p_rrd+i, p_rrh, p_gp, pool_cans, n_can);
                }
                // printf("%d-%d-%d: fragment: %d\n", (p_rrd+i)->date.y, (p_rrd+i)->date.m, (p_rrd+i)->date.d, fragment);
            }
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

int Toggle_CONTINUITY(
    struct df_rr_h *p_rrh,
    struct df_rr_d *p_rrd,
    struct Para_global *p_gp,
    int ndays_h,
    int pool_cans[],
    int WD // Wet-dry status matching: strict:0; flexible:1
){
    /*****************
     * Description:
     *      wet-dry status continuity matching check
     * Parameters:
     *      p_rrh: pointing to df_rr_hourly struct array (all the hourly rr observations);
     *      p_rrd: pointing to target day (to be disaggregated);
     *      p_gp: pointing to global parameter struct;
     *      ndays_h: number of days in hourly rr data;
     *      pool_cans: the pool of candidate index;
     *      WD: matching flexcibility, global parameter
     * Output:
     *      - the number of candidates (pool size)
     *      - bring back the pool_cans array
     * ***************/
    int i, j, k, skip, toggle_WD;
    // toggle_WD: whether wet-dry status match; 1: match; 0: not match
    int n_cans=0;  // number of candidates fullfilling the CONTINUITY criteria (output)
    skip = (p_gp->CONTINUITY - 1) / 2; // p_gp->CONTINUITY == 1: skip=0; p_gp->CONTINUITY == 3: skip=1
    
    for (k=skip; k < ndays_h-skip; k++) { 
        // iterate each day in hourly observations
        toggle_WD = 1;
        for (i=0-skip; i < 1+skip; i++) {
            // here we have to compare vectors between candidates and target; 
            //      CONTINUITY=1: skip=0; CONTINUITY=3: skip=1; 
            for (j=0; j < p_gp->N_STATION; j++){
                if (WD == 0) {
                    /*strict wet-dry matching*/
                    // the wet-dry status in each vector should be completely the same with each other.
                    if (
                        !(
                            ((p_rrd + i)->p_rr[j] > 0.0) == ((p_rrh + k + i)->rr_d[j] > 0.0)
                        )
                    ) {
                        toggle_WD = 0;break;
                    } else {
                        continue;
                    }
                } else {
                    // WD == 1
                    /* flexible wet-dry status matching*/
                    if (
                        ((p_rrd + i)->p_rr[j] > 0.0) && ((p_rrh + k + i)->rr_d[j] <= 0.0)
                    ) {
                        toggle_WD = 0;break;
                    }
                }
            }
        }
        if (toggle_WD == 1) {
            *(pool_cans + n_cans) = k;
            n_cans++;
            // printf("n_cans: %d\n", n_cans);
        }
    }
    return n_cans;
}

int Toogle_CP(
    struct Date date,
    struct df_cp *p_cp,
    int nrow_cp
){
    /*************
     * Description:
     *      derive the cp value (class) of the day based on date stamp (y-m-d)
     * Parameters:
     *      date: a Date struct, conaining y, m and d
     *      p_cp: pointing to the cp data struct array
     *      nrow_cp: total rows of cp observations
     * Output:
     *      return the derived cp value
     * **********/
    int i;
    int cp = -1;
    for (i = 0; i < nrow_cp; i++) {
        if (
            (p_cp+i)->date.y == date.y && (p_cp+i)->date.m == date.m && (p_cp+i)->date.d == date.d
        ) {
            cp = (p_cp+i)->cp;
            break; // terminate the loop directly
        }
    }
    if (cp == -1) {
        printf(
            "Program terminated: cannot find the cp class for the date %d-%02d-%02d\n",
            date.y, date.m, date.d
        );
        exit(2);
    }
    return cp;
}

int kNN_sampling(
    struct df_rr_d *p_rrd,
    struct df_rr_h *p_rrh,
    struct Para_global *p_gp,
    int pool_cans[],
    int n_can
){
    /**************
     * Description:
     *      - compute the manhattan distance of rr between target and candidate days
     *      - sort the distance in the increasing order
     *      - select the sqrt(n_can) largest distance
     *      - weights defined as inverse of distance
     *      - sample one distance 
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
    double *distance;  // the distance between target day and candidate days
    distance = malloc(n_can * sizeof(double));
    int size_pool; // the k in kNN
    int index_out; // the output of this function: the sampled fragment from candidates pool
    /**manhattan distance*/
    for (i=0; i<n_can; i++){
        *(distance+i) = 0.0;
        for (j=0; j<p_gp->N_STATION; j++){
            *(distance+i) += fabs(p_rrd->p_rr[j] - (p_rrh + pool_cans[i])->rr_d[j]);
        }
        // printf("candidate index: %d, distance: %.2f\n", pool_cans[i], *(distance+i));
    }
    // sort the distance in the increasing order
    for (i=0; i<n_can-1; i++) {
        for (j=i+1; j<n_can; j++) {
            if (distance[i] > distance[j]) {
                temp_c = pool_cans[i]; pool_cans[i] = pool_cans[j]; pool_cans[j] = temp_c;
                temp_d = distance[i]; distance[i] = distance[j]; distance[j] = temp_d;
            }
        }
    }
    /*
    printf("--------------after sorting---------\n");
    for (i=0; i<n_can;i++) {
        printf("candidate index: %d, distance: %.2f\n", pool_cans[i], *(distance+i));
    }
    */
    if (distance[0] <= 0.0) {
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
         *      the weight is defined as distance inverse
         * dynamic memory allocation for the double array - weights
         * */
        double *weights;
        weights = malloc(size_pool * sizeof(double)); // a double array with the size of size_pool
        double w_sum=0.0; // the sum of the inversed distances
        for (i=0; i<size_pool; i++){
            *(weights+i) = 1/distance[i];
            w_sum += 1/distance[i];
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
        /* generate a random number, then an fragments index*/
        int toggle = 0;
        while (toggle == 0)
        {
            toggle = 1;
            index_out = weight_cdf_sample(size_pool, pool_cans, weights_cdf);
            for (j = 0; j < p_gp->N_STATION; j++)
            {
                if (p_rrd->p_rr[j] > 0.0 && (p_rrh + index_out)->rr_d[j] <= 0.0)
                {
                    toggle = 0;
                    break;
                }
            }
        }    
        free(weights);
        free(weights_cdf);
    }
    free(distance);
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
    int j,h;
    for (j = 0; j < p_gp->N_STATION; j++)
    {
        if (p_out->rr_d[j] > 0.0)
        {
            for (h = 0; h < 24; h++)
            {
                p_out->rr_h[j][h] = p_out->rr_d[j] * (p_rrh + fragment)->rr_h[j][h] / (p_rrh + fragment)->rr_d[j];
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
    /*
    int toggle = 1;
    for (j=0; j<p_gp->N_STATION;j++) {
        if (p_out->rr_d[j] > 0.0 && (p_rrh + fragment)->rr_d[j] <= 0.0 ) {
            toggle = 0;
            break;
        }
    }
    if (toggle == 1) {
        for (j=0; j<p_gp->N_STATION; j++) {
            if (p_out->rr_d[j] > 0.0) {
                for (h=0; h<24; h++) {
                    p_out->rr_h[j][h] = p_out->rr_d[j] * (p_rrh + fragment)->rr_h[j][h] / (p_rrh + fragment)->rr_d[j];
                }
            } else {
                // no rain at station j
                for (h=0; h<24; h++) {p_out->rr_h[j][h] = 0.0;}
            }
        }
    } else {
        printf("The target day %d-%02d-%02d disaggregation failed, please check or retry!\n---Programm terminated!", 
            p_out->date.y, p_out->date.m, p_out->date.d);
        exit(0);
    } */
}