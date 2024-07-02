#ifndef FUNC_RECURSIVE
#define FUNC_RECURSIVE

extern FILE *p_SSIM;

void kNN_MOF_SSIM_Recursive(
    struct df_rr_h *p_rrh,
    struct df_rr_d *p_rrd,
    struct df_cp *p_cp,
    struct Para_global *p_gp,
    struct df_coor *p_coor,
    int nrow_rr_d,
    int ndays_h,
    int nrow_cp);

void kNN_SSIM_sampling_recursive(
    struct df_rr_d *p_rrd,
    struct df_rr_h *p_rrh,
    struct Para_global *p_gp,
    struct df_rr_h *p_out,
    int index_target,
    int pool_cans[],
    int n_can,
    int skip
);


void Initialize_output(
    struct df_rr_h *p_out,
    struct Para_global *p_gp,
    struct df_rr_d *p_rrd,
    int index_target
);

void Fragment_assign_recursive(
    struct df_rr_h *p_rrh,
    struct df_rr_h *p_out,
    struct df_rr_d *p_rrd,
    struct Para_global *p_gp,
    int index_target,
    int fragment);
    

#endif