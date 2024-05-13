#ifndef FUNC_KNN
#define FUNC_KNN


void similarity_sorting(
    double *similarity,
    int *pool_cans,
    int order,
    int n_can
);

void similarity_weight(
    double *similarity,
    int *pool_cans,
    int order,
    int n_can,
    int *size_pool,
    double **weights
);

void kNN_sampling(
    double *similarity,
    int *pool_cans,
    int order,
    int n_can,
    int run,
    int *index_fragment
);


double get_random();

int weight_cdf_sample(
    int size_pool,
    int pool_cans[],
    double *weights_cdf    
);

#endif