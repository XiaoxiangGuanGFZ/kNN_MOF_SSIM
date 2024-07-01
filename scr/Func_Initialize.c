/*
 * SUMMARY:      Func_Initialize.c
 * USAGE:        assign the seasonality, cp type and class 
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Apr-2024
 * DESCRIPTION:  MOD is based several conditions: seasonality, circulation pattern
 *               therefore, we assign each day a season (summer or winter) and a cp class
 * DESCRIP-END.
 * FUNCTIONS:    initialize_dfrr_d(); initialize_dfrr_h(); Toogle_CP();
 *               CP_classes();
 * COMMENTS:
 * 
 *
 */

/************
 * 
 * 
********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "def_struct.h"
#include "Func_Initialize.h"

void initialize_dfrr_d(
    struct Para_global *p_gp,
    struct df_rr_d *p_rr_d,
    struct df_cp *p_cp,
    int nrow_rr_d,
    int nrow_cp
)
{
    if (strncmp(p_gp->MONTH, "TRUE", 4) == 0 && strncmp(p_gp->SEASON, "TRUE", 4) == 0) 
    {
        printf("The disaggregation can only be conditioned on either MONTH or SEASON!\n");
        exit(1);
    }

    /*******
     * assign each day the cp value
     */
    int N_CP_CLASS;
    int N_SM_CLASS;
    if (strncmp(p_gp->T_CP, "TRUE", 4) == 0)
    {
        N_CP_CLASS = CP_classes(p_cp, nrow_cp);
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_d + i)->cp = Toogle_CP((p_rr_d + i)->date, p_cp, nrow_cp);
        }
    }
    else
    {
        N_CP_CLASS = 0;
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_d + i)->cp = 0;
        }
    }

    /*******
     * assign each day the season (summer or winter) value
     */
    if (strncmp(p_gp->SEASON, "TRUE", 4) == 0) {
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            if ((p_rr_d + i)->date.m >= p_gp->SUMMER_FROM && (p_rr_d + i)->date.m <= p_gp->SUMMER_TO)
            {
                (p_rr_d + i)->SM = 1; // summer
            } else {
                (p_rr_d + i)->SM = 0; // winter
            }
        }
        N_SM_CLASS = 2;
    }
    else if (strncmp(p_gp->MONTH, "TRUE", 4) == 0)
    {
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_d + i)->SM = (p_rr_d + i)->date.m - 1;
        }
        N_SM_CLASS = 12;
    } else {
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_d + i)->SM = 0;
        }
        N_SM_CLASS = 0;
    }

    if (N_SM_CLASS > 0 && N_CP_CLASS > 0)
    {
        p_gp->CLASS_N = N_SM_CLASS * N_CP_CLASS;
    } else if (N_SM_CLASS > 0 && N_CP_CLASS == 0)
    {
        p_gp->CLASS_N = N_SM_CLASS;
    } else if (N_SM_CLASS == 0 && N_CP_CLASS > 0)
    {
        p_gp->CLASS_N = N_CP_CLASS;
    } else {
        p_gp->CLASS_N = 0;
    }

    /******************
     * time series is classified based on following combinations:
     * - month alone
     * - season alone
     * - month and cp
     * - season and cp
     * - cp alone
     * - nothing
     * ****/
    if (N_CP_CLASS > 0 && N_SM_CLASS > 0)
    {
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_d + i)->class = ((p_rr_d + i)->cp - 1) + N_CP_CLASS * (p_rr_d + i)->SM;
        }
    }
    else if (N_CP_CLASS > 0 && N_SM_CLASS == 0)
    {
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_d + i)->class = (p_rr_d + i)->cp - 1;
        }
    }
    else if (N_CP_CLASS == 0 && N_SM_CLASS > 0)
    {
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_d + i)->class = (p_rr_d + i)->SM;
        }
    }
    else
    {
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_d + i)->class = 0;
        }
    }
}


void initialize_dfrr_h(
    struct Para_global *p_gp,
    struct df_rr_h *p_rr_h,
    struct df_cp *p_cp,
    int nrow_rr_d,
    int nrow_cp
)
{
    if (strncmp(p_gp->MONTH, "TRUE", 4) == 0 && strncmp(p_gp->SEASON, "TRUE", 4) == 0)
    {
        printf("The disaggregation can only be conditioned on either MONTH or SEASON!\n");
        exit(1);
    }

    /*******
     * assign each day the cp value
     */
    int N_CP_CLASS, N_SM_CLASS;
    if (strncmp(p_gp->T_CP, "TRUE", 4) == 0)
    {
        N_CP_CLASS = CP_classes(p_cp, nrow_cp);
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_h + i)->cp = Toogle_CP((p_rr_h + i)->date, p_cp, nrow_cp);
        }
    }
    else
    {
        N_CP_CLASS = 0;
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_h + i)->cp = 0;
        }
    }

    /*******
     * assign each day the season (summer or winter) value
    */
    if (strncmp(p_gp->SEASON, "TRUE", 4) == 0) {
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            if ((p_rr_h + i)->date.m >= p_gp->SUMMER_FROM && (p_rr_h + i)->date.m <= p_gp->SUMMER_TO)
            {
                (p_rr_h + i)->SM = 1; // summer
            } else {
                (p_rr_h + i)->SM = 0; // winter
            }
        }
        N_SM_CLASS = 2;
    }
    else if (strncmp(p_gp->MONTH, "TRUE", 4) == 0)
    {
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_h + i)->SM = (p_rr_h + i)->date.m - 1;
        }
        N_SM_CLASS = 12;
    } else {
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_h + i)->SM = 0;
        }
        N_SM_CLASS = 0;
    }

    /******************
     * time series is classified based on following combinations: 
     * - month alone
     * - season alone
     * - month and cp
     * - season and cp
     * - cp alone
     * - nothing
     * ****/

    if (N_CP_CLASS > 0 && N_SM_CLASS > 0)
    {
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_h + i)->class = ((p_rr_h + i)->cp - 1) + N_CP_CLASS * (p_rr_h + i)->SM;
        }
    }
    else if (N_CP_CLASS > 0 && N_SM_CLASS == 0)
    {
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_h + i)->class = (p_rr_h + i)->cp - 1;
        }
    }
    else if (N_CP_CLASS == 0 && N_SM_CLASS > 0)
    {
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_h + i)->class = (p_rr_h + i)->SM;
        }
    }
    else
    {
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_h + i)->class = 0;
        }
    }
}



int Toogle_CP(
    struct Date date,
    struct df_cp *p_cp,
    int nrow_cp
)
{
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

int CP_classes(
    struct df_cp *p_cp,
    int nrow_cp
)
{
    // derive the number of circulation pattern classes
    int cp_max=0;
    for (size_t i = 0; i < nrow_cp; i++)
    {
        if ((p_cp + i)->cp > cp_max)
        {
            cp_max = (p_cp + i)->cp;
        }
    }
    return cp_max;
}


/**********************
 * view_class:
 * print the classes and corresponding sample sizes of both 
 * daily (to disaggregate) and hourly (fragments donor)
 * time series of the variable.
 * 
 * *********************/

void view_class_rrd(
    struct df_rr_d *p_rr_d,
    int nrow_rr_d
)
{
    int n_classes = 0;
    for (size_t i = 0; i < nrow_rr_d; i++)
    {
        if ((p_rr_d + i)->class > n_classes)
        {
            n_classes = (p_rr_d + i)->class;
        }
    }
    n_classes += 1;  // the total number of classes the time series is categorized into. 

    int *counts;
    counts = (int *)malloc(sizeof(int) * n_classes);
    for (size_t i = 0; i < n_classes; i++)
    {
        // initialize 
        *(counts + i) = 0;
    }

    for (size_t t = 0; t < n_classes; t++)
    {
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            if ((p_rr_d + i)->class == t)
            {
                *(counts + t) += 1;
            }
        }
    }
    
    /**********************************
     * print the counts of each class to screen
     */
    printf("* class-counts:\n   - class: "); fprintf(p_log, "* class-counts:\n   - class: ");
    for (size_t t = 0; t < n_classes; t++)
    {
        printf("%5ld ", t + 1); fprintf(p_log, "%5ld ", t + 1);
    }
    printf("\n"); fprintf(p_log, "\n");

    printf("   - count: "); fprintf(p_log, "   - count: ");
    for (size_t t = 0; t < n_classes; t++)
    {
        printf("%5d ", *(counts + t)); fprintf(p_log, "%5d ", *(counts + t));
    }
    printf("\n"); fprintf(p_log, "\n");
    
}


void view_class_rrh(
    struct df_rr_h *p_rr_h,
    int nrow_rr_d
)
{
    int n_classes = 0;
    for (size_t i = 0; i < nrow_rr_d; i++)
    {
        if ((p_rr_h + i)->class > n_classes)
        {
            n_classes = (p_rr_h + i)->class;
        }
    }
    n_classes += 1;  // the total number of classes the time series is categorized into. 

    int *counts;
    counts = (int *)malloc(sizeof(int) * n_classes);
    for (size_t i = 0; i < n_classes; i++)
    {
        // initialize 
        *(counts + i) = 0;
    }

    for (size_t t = 0; t < n_classes; t++)
    {
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            if ((p_rr_h + i)->class == t)
            {
                *(counts + t) += 1;
            }
        }
    }
    
    /**********************************
     * print the counts of each class to screen
     */
    printf("* class-counts:\n   - class: "); fprintf(p_log, "* class-counts:\n   - class: ");
    for (size_t t = 0; t < n_classes; t++)
    {
        printf("%5ld ", t + 1); fprintf(p_log, "%5ld ", t + 1);
    }
    printf("\n"); fprintf(p_log, "\n");

    printf("   - count: "); fprintf(p_log, "   - count: ");
    for (size_t t = 0; t < n_classes; t++)
    {
        printf("%5d ", *(counts + t)); fprintf(p_log, "%5d ", *(counts + t));
    }
    printf("\n"); fprintf(p_log, "\n");
}

/*********************
 * coordinates of weather sites 
 * in this multi-site disaggregation algorithm
 * ****************/
void initialize_df_coor(
    struct Para_global *p_gp,
    struct df_coor **p_coor,
    int nrow_coor
)
{
    if (nrow_coor != p_gp->N_STATION)
    {
        printf("FP_COOR: rainsite number contradiction between FP_COOR and data files!\n");
        exit(1);
    } else {
        double *distance; double d_temp;
        int *id; int id_temp;
        int n_neighbors;
        n_neighbors = nrow_coor - 1;  // number of neighbouring rain site in total
        distance = (double *)malloc(sizeof(double) * n_neighbors);
        id = (int *)malloc(sizeof(int) * n_neighbors);

        /**************
         * iterate each rain site, obtain a rank vector of
         * distance with all the neighbours (in increasing order)
         */
        for (size_t i = 0; i < nrow_coor; i++)
        {
            (*p_coor + i)->neighbors = (int *)malloc(sizeof(int) * n_neighbors);

            /*****
             * calculate the distance between target rain site with its neighbours
             * every rain site has n-1 neighbours (excluding itself)
             */
            int t = 0;
            for (size_t j = 0; j < nrow_coor; j++)
            {
                if (i != j)
                {
                    *(distance + t) = COOR_distance(
                        (*p_coor + i)->lon, (*p_coor + i)->lat, 
                        (*p_coor + j)->lon, (*p_coor + j)->lat
                    );
                    *(id + t) = j;
                    t += 1;
                }                
            }

            // sorting the distance vector in the increasing order
            for (size_t j = 0; j < n_neighbors - 1; j++)
            {
                for (size_t k = j + 1; k < n_neighbors; k++)
                {
                    if (*(distance + j) > *(distance + k))
                    {
                        d_temp = *(distance + j);
                        *(distance + j) = *(distance + k);
                        *(distance + k) = d_temp;
                        id_temp = *(id + j);
                        *(id + j) = *(id + k);
                        *(id + k) = id_temp;
                    }
                }
            }

            // assign the rank to the structure
            for (size_t j = 0; j < n_neighbors; j++)
            {
                *((*p_coor + i)->neighbors + j) = *(id + j);
            }
        }
        free(id);
        free(distance);
    }
    
}

double COOR_distance(
    double lon1,
    double lat1,
    double lon2,
    double lat2
)
{
    double d;
    d = pow((lon1 - lon2), 2) + pow(lat1 - lat2, 2);
    return d;
}

