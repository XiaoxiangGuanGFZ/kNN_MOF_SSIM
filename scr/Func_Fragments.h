
#ifndef FUNC_FRAGMENTS
#define FUNC_FRAGMENTS


int Toggle_WD(
    int N_STATION,
    double *p_rr_d
);

int Toggle_CONTINUITY(
    struct df_rr_h *p_rrh,
    struct df_rr_d *p_rrd,
    struct Para_global *p_gp,
    int ndays_h,
    int pool_cans[]
);


void Fragment_assign(
    struct df_rr_h *p_rrh,
    struct df_rr_h *p_out,
    struct Para_global *p_gp,
    struct df_coor *p_coor,
    int fragment
);


#endif