#ifndef Func_Initialize
#define Func_Initialize

extern FILE *p_log;  // file pointer pointing to log file

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

void initialize_L(
    struct df_rr_h *p_rrh,
    struct df_rr_d *p_rrd,
    struct Para_global *p_gp,
    int nrow_rr_d,
    int ndays_h
);

void view_class_rrd(
    struct df_rr_d *p_rr_d,
    int nrow_rr_d
);

void view_class_rrh(
    struct df_rr_h *p_rr_h,
    int nrow_rr_d
);


#endif