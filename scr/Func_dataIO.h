#ifndef FUNC_DATAIO
#define FUNC_DATAIO

void import_global(
    char fname[], struct Para_global *p_gp
);

void removeLeadingSpaces(char *str);

int import_dfrr_d(
    char FP_daily[], 
    int N_STATION,
    struct df_rr_d *p_rr_d
);

int import_df_coor(
    char fname[],
    struct df_coor *p_df_coor
);

int import_dfrr_h(
    char FP_hourly[], 
    int N_STATION,
    struct df_rr_h *p_rr_h
);

int import_df_cp(
    char fname[],
    struct df_cp *p_df_cp
);

void Write_df_rr_h(
    struct df_rr_h *p_out,
    struct Para_global *p_gp,
    FILE *p_FP_OUT,
    int run
);

#endif