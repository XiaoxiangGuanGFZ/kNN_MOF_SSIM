/*
 * SUMMARY:      Func_Prepro.c
 * USAGE:        preprocessing of rainfall before disaggregation
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    May-2024
 * DESCRIPTION:  preprocessing: normalization or standardization; 
 *               considering the high skewwness of rainfall data
 * DESCRIP-END.
 * FUNCTIONS:    kNN_MOF_SSIM(); Toggle_WD(); kNN_SSIM_sampling();
 *               get_random(); 
 *
 * COMMENTS:
 * normalization: transform the date into the range of [0, 1];
 * standardization: scale the values around mean with a unit standard deviation;
 * REFERENCEs:
 * 
 */

/*******************************************************************************
 * VARIABLEs:
 * 
 *****/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "def_struct.h"
#include "Func_Prepro.h"

void Normalize_rain(
    struct Para_global *p_gp,
    struct df_rr_d *p_rr_d,
    struct df_rr_h *p_rr_h,
    int nrow_d,
    int nrow_h)
{
    int N;
    double max = 0.0; 
    double min = 0.0;
    double range;
    N = p_gp->N_STATION;
    for (size_t i = 0; i < nrow_d; i++)
    {
        for (size_t j = 0; j < N; j++)
        {
            if ((p_rr_d + i)->p_rr[j] > max)
            {
                max = (p_rr_d + i)->p_rr[j];
            }
        }
    }

    for (size_t i = 0; i < nrow_h; i++)
    {
        for (size_t j = 0; j < N; j++)
        {
            if ((p_rr_h + i)->rr_d[j] > max)
            {
                max = (p_rr_h + i)->rr_d[j];
            }
        }
    }

    range = max - min;
    if (range <= 0.0)
    {
        printf("Error: in normalization, max == min! \n");
        exit(2);
    }

    for (size_t i = 0; i < nrow_d; i++)
    {
        (p_rr_d + i)->p_rr_pre = (double *)malloc(sizeof(double) * N);
        for (size_t j = 0; j < N; j++)
        {
            if ((p_rr_d + i)->p_rr[j] > 0.0)
            {
                (p_rr_d + i)->p_rr_pre[j] = ((p_rr_d + i)->p_rr[j] - min) / range;
            } else {
                (p_rr_d + i)->p_rr_pre[j] = 0.0;
            }
        }
    }
    for (size_t i = 0; i < nrow_h; i++)
    {
        (p_rr_h + i)->rr_d_pre = (double *)malloc(sizeof(double) * N);
        for (size_t j = 0; j < N; j++)
        {
            if ((p_rr_h + i)->rr_d[j] > 0.0)
            {
                (p_rr_h + i)->rr_d_pre[j] = ((p_rr_h + i)->rr_d[j] - min) / range;
            } else {
                (p_rr_h + i)->rr_d_pre[j] = 0.0;
            }
        }
    }
}

void Standardize_rain(
    struct Para_global *p_gp,
    struct df_rr_d *p_rr_d,
    struct df_rr_h *p_rr_h,
    int nrow_d,
    int nrow_h)
{
    double sum, mean, sd;
    int N; 
    N = p_gp->N_STATION;
    int counts;
    counts = 0;
    sum = 0.0;
    for (size_t i = 0; i < nrow_d; i++)
    {
        for (size_t j = 0; j < N; j++)
        {
            if ((p_rr_d + i)->p_rr[j] > 0.0)
            {
                sum += (p_rr_d + i)->p_rr[j];
                counts += 1;
            }
        }
    }
    for (size_t i = 0; i < nrow_h; i++)
    {
        for (size_t j = 0; j < N; j++)
        {
            if ((p_rr_h + i)->rr_d[j] > 0.0)
            {
                sum += (p_rr_h + i)->rr_d[j];
                counts += 1;
            }
        }
    }

    mean = (double) (sum / counts);

    sum = 0.0;
    for (size_t i = 0; i < nrow_d; i++)
    {
        for (size_t j = 0; j < N; j++)
        {
            if ((p_rr_d + i)->p_rr[j] > 0.0)
            {
                sum += pow((p_rr_d + i)->p_rr[j] - mean, 2);
            }
        }
    }
    for (size_t i = 0; i < nrow_h; i++)
    {
        for (size_t j = 0; j < N; j++)
        {
            if ((p_rr_h + i)->rr_d[j] > 0.0)
            {
                sum += pow((p_rr_h + i)->rr_d[j] - mean, 2);
            }
        }
    }

    sd = sqrt(sum / (double) counts);

    for (size_t i = 0; i < nrow_d; i++)
    {
        (p_rr_d + i)->p_rr_pre = (double *)malloc(sizeof(double) * N);
        for (size_t j = 0; j < N; j++)
        {
            if ((p_rr_d + i)->p_rr[j] > 0.0)
            {
                (p_rr_d + i)->p_rr_pre[j] = ((p_rr_d + i)->p_rr[j] - mean) / sd;
            } else {
                (p_rr_d + i)->p_rr_pre[j] = 0.0;
            }
        }
    }

    for (size_t i = 0; i < nrow_h; i++)
    {
        (p_rr_h + i)->rr_d_pre = (double *)malloc(sizeof(double) * N);
        for (size_t j = 0; j < N; j++)
        {
            if ((p_rr_h + i)->rr_d[j] > 0.0)
            {
                (p_rr_h + i)->rr_d_pre[j] = ((p_rr_h + i)->rr_d[j] - mean) / sd;
            } else {
                (p_rr_h + i)->rr_d_pre[j] = 0.0;
            }
        }
    }
}

