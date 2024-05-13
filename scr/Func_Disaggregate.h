#ifndef FUNC_DISAGGREGATE
#define FUNC_DISAGGREGATE

extern FILE *p_SSIM;

void kNN_MOF_SSIM(
    struct df_rr_h *p_rrh,
    struct df_rr_d *p_rrd,
    struct df_cp *p_cp,
    struct Para_global *p_gp,
    struct df_coor *p_coor,
    int nrow_rr_d,
    int ndays_h,
    int nrow_cp
);


void kNN_SSIM_sampling(
    struct df_rr_d *p_rrd,
    struct df_rr_h *p_rrh,
    struct Para_global *p_gp,
    int index_target,
    int pool_cans[],
    int n_can,
    int skip,
    int run,
    int *index_fragment
);


#endif