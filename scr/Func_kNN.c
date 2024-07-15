

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <sys/time.h>
#include <time.h>
#include "def_struct.h"
#include "Func_kNN.h"
#include "Func_SSIM.h"
#include "Func_Disaggregate.h"
#include "Func_dataIO.h"


void similarity_sorting(
    double *similarity,
    int *pool_cans,
    int order,
    int n_can)
{
    int i, j;
    int temp_c; // temporary variable during sorting
    double temp_d;

    if (order == 1)
    {
        // sort in the decreasing order
        for (i = 0; i < n_can - 1; i++)
        {
            for (j = i + 1; j < n_can; j++)
            {
                if (similarity[i] < similarity[j])
                {
                    temp_c = pool_cans[i];
                    pool_cans[i] = pool_cans[j];
                    pool_cans[j] = temp_c;
                    temp_d = similarity[i];
                    similarity[i] = similarity[j];
                    similarity[j] = temp_d;
                }
            }
        }
    }
    else
    {
        // sort in the increasing order
        for (i = 0; i < n_can - 1; i++)
        {
            for (j = i + 1; j < n_can; j++)
            {
                if (similarity[i] > similarity[j])
                {
                    temp_c = pool_cans[i];
                    pool_cans[i] = pool_cans[j];
                    pool_cans[j] = temp_c;
                    temp_d = similarity[i];
                    similarity[i] = similarity[j];
                    similarity[j] = temp_d;
                }
            }
        }
    }
}

void similarity_weight(
    double *similarity,
    int *pool_cans,
    int order,
    int n_can,
    int *size_pool,
    double **weights)
{
    int i;
    /******
     * *size_pool:
     * the size of candidate pool in kNN algorithm
     *      the range of size_pool:
     *      [2, n_can]
     ****/
    // int size_pool;
    *size_pool = (int)sqrt(n_can) + 1;

    *weights = (double *)malloc(*size_pool * sizeof(double)); // a double array with the size of size_pool
    double w_sum = 0.0;
    if (order == 1)
    {
        /*****
         * like: SSIM;
         * larger SSIM, higher weight
         * **/
        for (i = 0; i < *size_pool; i++)
        {
            *(*weights + i) = similarity[i] + 1;
            w_sum += similarity[i] + 1;
        }
    }
    else
    {
        /******
         * like: Manhattan distance;
         * larger distance, lower weight
         * ***/
        for (i = 0; i < *size_pool; i++)
        {
            *(*weights + i) = 1.0 / (similarity[i] + 1); // inverse distance
            w_sum += 1.0 / (similarity[i] + 1);
        }
    }

    for (i = 0; i < *size_pool; i++)
    {
        *(*weights + i) /= w_sum; // reassignment
    }
}

void kNN_sampling(
    double *similarity,
    int *pool_cans,
    int order,
    int n_can,
    int run,
    int *index_fragment)
{

    similarity_sorting(similarity, pool_cans, order, n_can);
    int size_pool;
    double *weights;
    similarity_weight(similarity, pool_cans, order, n_can, &size_pool, &weights);

    /* compute the empirical cdf for weights (vector) */
    double *weights_cdf;
    weights_cdf = malloc(size_pool * sizeof(double));
    *(weights_cdf + 0) = weights[0]; // initialization
    for (size_t i = 1; i < size_pool; i++)
    {
        *(weights_cdf + i) = *(weights_cdf + i - 1) + weights[i];
    }
    /* generate a random number, then select the fragments index */
    for (size_t t = 0; t < run; t++)
    {
        index_fragment[t] = weight_cdf_sample(size_pool, pool_cans, weights_cdf);
    }

    free(weights);
    free(weights_cdf);
}

double get_random()
{
    int random_number = rand();
    // printf("Random number: %d\n", random_number);
    return ((double)random_number / (double)RAND_MAX);
}

int weight_cdf_sample(
    int size_pool,
    int pool_cans[],
    double *weights_cdf)
{
    int i;
    double rd = 0.0; // a random decimal value between 0.0 and 1.0
    int index_out;   // the output of this function: the sampled fragment from candidates pool

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

void seed_random() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned int seed = (unsigned int)(tv.tv_sec * 1000000 + tv.tv_usec);
    srand(seed);  // Use both seconds and microseconds for seed
    // printf("Seed: %u\n", seed);  // Debug statement to print the seed
}
