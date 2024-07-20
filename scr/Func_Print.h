#ifndef FUNC_PRINT
#define FUNC_PRINT

extern FILE *p_log;  // file pointer pointing to log file
extern int FLAG_LOG;

void Print_gp(
    struct Para_global *p_gp
);

void Print_cp(
    struct df_cp *df_cps,
    int nrow_cp
);

void Print_dly(
    struct df_rr_d *df_dly,
    struct Para_global *p_gp,
    int nrow_rr_d
);

void Print_hly(
    struct df_rr_h *df_hly,
    int ndays_h
);


#endif


