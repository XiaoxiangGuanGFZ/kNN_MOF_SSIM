#ifndef Func_Initialize
#define Func_Initialize

void initialize_dfrr_d(
    struct Para_global *p_gp,
    struct df_rr_d *p_rr_d,
    struct df_cp *p_cp,
    int nrow_rr_d,
    int nrow_cp
);

void initialize_dfrr_h(
    struct Para_global *p_gp,
    struct df_rr_h *p_rr_h,
    struct df_cp *p_cp,
    int nrow_rr_d,
    int nrow_cp
);

int Toogle_CP(
    struct Date date,
    struct df_cp *p_cp,
    int nrow_cp
);

int CP_classes(
    struct df_cp *p_cp,
    int nrow_cp
);

#endif