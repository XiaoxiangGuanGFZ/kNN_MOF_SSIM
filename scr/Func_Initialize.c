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
    /*******
     * assign each day the cp value
    */
    if (strncmp(p_gp->T_CP, "TRUE", 4) == 0)
    {
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_d + i)->cp =
                Toogle_CP(
                    (p_rr_d + i)->date,
                    p_cp,
                    nrow_cp);
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
                (p_rr_d + i)->season = 1; // summer
            } else {
                (p_rr_d + i)->season = 0; // winter
            }
        }
    }

    /**********
     * assign each day a class value based on different combination
     * - CP and SEASON
     * - CP only
     * - SEASON only
     * - neither
    */
    if (strncmp(p_gp->T_CP, "TRUE", 4) == 0 && strncmp(p_gp->SEASON, "TRUE", 4) != 0)
    {
        p_gp->CLASS_N = CP_classes(p_cp, nrow_cp);
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_d + i)->class = (p_rr_d + i)->cp - 1;
        }
    } else if (strncmp(p_gp->T_CP, "TRUE", 4) != 0 && strncmp(p_gp->SEASON, "TRUE", 4) != 0)
    {
        p_gp->CLASS_N = 1;
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_d + i)->class = 0;
        }
    } else if (strncmp(p_gp->T_CP, "TRUE", 4) != 0 && strncmp(p_gp->SEASON, "TRUE", 4) == 0)
    {
        p_gp->CLASS_N = 2;
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_d + i)->class = (p_rr_d + i)->season;
        }
    } else {
        p_gp->CLASS_N = CP_classes(p_cp, nrow_cp) * 2;
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_d + i)->class = ((p_rr_d + i)->cp - 1) * 2 + (p_rr_d + i)->season;
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
    /*******
     * assign each day the cp value
    */
    if (strncmp(p_gp->T_CP, "TRUE", 4) == 0)
    {
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_h + i)->cp =
                Toogle_CP(
                    (p_rr_h + i)->date,
                    p_cp,
                    nrow_cp);
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
                (p_rr_h + i)->season = 1; // summer
            } else {
                (p_rr_h + i)->season = 0; // winter
            }
        }
    }

    /**********
     * assign each day a class value based on different combination
     * - CP and SEASON
     * - CP only
     * - SEASON only
     * - neither
    */
    if (strncmp(p_gp->T_CP, "TRUE", 4) == 0 && strncmp(p_gp->SEASON, "TRUE", 4) != 0)
    {
        p_gp->CLASS_N = CP_classes(p_cp, nrow_cp);
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_h + i)->class = (p_rr_h + i)->cp - 1;
        }
    } else if (strncmp(p_gp->T_CP, "TRUE", 4) != 0 && strncmp(p_gp->SEASON, "TRUE", 4) != 0)
    {
        p_gp->CLASS_N = 1;
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_h + i)->class = 0;
        }
    } else if (strncmp(p_gp->T_CP, "TRUE", 4) != 0 && strncmp(p_gp->SEASON, "TRUE", 4) == 0)
    {
        p_gp->CLASS_N = 2;
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_h + i)->class = (p_rr_h + i)->season;
        }
    } else {
        p_gp->CLASS_N = CP_classes(p_cp, nrow_cp) * 2;
        for (size_t i = 0; i < nrow_rr_d; i++)
        {
            (p_rr_h + i)->class = ((p_rr_h + i)->cp - 1) * 2 + (p_rr_h + i)->season;
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

void initialize_L(
    struct df_rr_h *p_rrh,
    struct df_rr_d *p_rrd,
    struct Para_global *p_gp,
    int nrow_rr_d,
    int ndays_h
)
{
    double L = 0.0;
    double rr_temp;
    for (size_t i = 0; i < nrow_rr_d; i++)
    {
        for (size_t j = 0; j < p_gp->N_STATION; j++)
        {
            rr_temp = (p_rrd + i)->p_rr[j];
            if (rr_temp > L)
            {
                L = rr_temp;
            }
        }
    }
    
    for (size_t i = 0; i < ndays_h; i++)
    {
        for (size_t j = 0; j < p_gp->N_STATION; j++)
        {
            rr_temp = (p_rrh + i)->rr_d[j];
            if (rr_temp > L)
            {
                L = rr_temp;
            }
        }
    }
    p_gp->L = L + 1;
}

