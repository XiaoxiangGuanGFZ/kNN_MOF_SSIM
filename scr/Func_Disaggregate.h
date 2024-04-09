#ifndef FUNC_DISAGGREGATE
#define FUNC_DISAGGREGATE

void kNN_MOF(
    struct df_rr_h *p_rrh,
    struct df_rr_d *p_rrd,
    struct df_cp *p_cp,
    struct Para_global *p_gp,
    int nrow_rr_d,
    int ndays_h,
    int nrow_cp
);

int Toggle_CONTINUITY(
    struct df_rr_h *p_rrh,
    struct df_rr_d *p_rrd,
    struct Para_global *p_gp,
    int ndays_h,
    int pool_cans[],
    int WD // Wet-dry status matching: strict:0; flexible:1
);

int Toogle_CP(
    struct Date date,
    struct df_cp *p_cp,
    int nrow_cp
);

int kNN_sampling(
    struct df_rr_d *p_rrd,
    struct df_rr_h *p_rrh,
    struct Para_global *p_gp,
    int pool_cans[],
    int n_can
);

double get_random();

int weight_cdf_sample(
    int size_pool,
    int pool_cans[],
    double *weights_cdf    
);

void Fragment_assign(
    struct df_rr_h *p_rrh,
    struct df_rr_h *p_out,
    struct Para_global *p_gp,
    int fragment
);


#endif