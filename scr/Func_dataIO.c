#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "def_struct.h"

void import_global(
    char fname[], struct Para_global *p_gp
){
    /**************
     * import the global parameters into memory for disaggregation algorithm
     * 
     * -- Parameters:
     *      fname: a string (1-D character array), file path and name of the global parameters
     * -- Output:
     *      return a structure containing the key fields
     * ********************/
    
    char row[MAXCHAR];
    FILE *fp;
    char *token;
    char *token2;
    int i;
    
    if ((fp=fopen(fname, "r")) == NULL) {
        printf("cannot open global parameter file: %s\n", fname);
        exit(1);
    }
    while (fgets(row, MAXCHAR, fp) != NULL)
    {
        // the fgets() function comes from <stdbool.h>
        // Reads characters from stream and stores them as a C string
        
        /***
         * removeLeadingSpaces():
         * remove all the leading white spaces in the string if exist,
         * otherwise do nothing!
        */
        removeLeadingSpaces(row);

        if (row != NULL && strlen(row) > 1) {
            /*non-empty row(string)*/
            if (row[0] != '#') {
                /* the first character of row should not be # */
                for (i = 0; i < strlen(row); i++)
                {
                    /* remove (or hide) all the characters after # */
                    if (row[i] == '#')
                    {
                        row[i] = '\0';
                        break;
                    }
                }
                // puts(row);
                /*assign the values to the parameter structure: key-value pairs*/
                token = strtok(row, ",");   // the first column: key
                token2 = strtok(NULL, ",\r\n");  // the second column: value
                // printf("token: %s\n", token);
                if (strncmp(token, "FP_DAILY", 8) == 0) {
                    strcpy(p_gp->FP_DAILY, token2);
                } else if (strncmp(token, "FP_CP", 5) == 0) {
                    strcpy(p_gp->FP_CP, token2);
                } else if (strncmp(token, "FP_HOURLY", 9) == 0) {
                    strcpy(p_gp->FP_HOURLY, token2);
                } else if (strncmp(token, "SEASON", 6) == 0) {
                    strcpy(p_gp->SEASON, token2);
                } else if (strncmp(token, "T_CP", 4) == 0) {
                    strcpy(p_gp->T_CP, token2);
                } else if (strncmp(token, "N_STATION", 9) == 0) {
                    p_gp->N_STATION = atoi(token2);
                } else if (strncmp(token, "WD", 2) == 0) {
                    p_gp->WD = atoi(token2);
                } else if (strncmp(token, "FP_OUT", 6) == 0) {
                    strcpy(p_gp->FP_OUT, token2);
                } else if (strncmp(token, "FP_LOG",6) == 0) {
                    strcpy(p_gp->FP_LOG, token2);
                } else if (strncmp(token, "CONTINUITY", 10) == 0) {
                    p_gp->CONTINUITY = atoi(token2);   
                } else {
                    printf(
                        "Error in opening global parameter file: unrecognized parameter field!"
                    );
                    exit(1);
                }
            }
        }
    }
    fclose(fp);
    
}
void removeLeadingSpaces(char *str) {
    if (str != NULL) {
        int i, j = 0;
        int len = strlen(str);
        // Find the first non-space character
        for (i = 0; i < len && isspace(str[i]); i++)
        {
            // Do nothing, just iterate until the first non-space character is found
        }
        // Shift the string to remove leading spaces
        for (; i < len; i++)
        {
            str[j++] = str[i];
        }
        // Null-terminate the modified string
        str[j] = '\0';
    }
}

int import_dfrr_d(
    char FP_daily[], 
    int N_STATION,
    struct df_rr_d *p_rr_d
) {
    /**************
     * Main:
     *  import daily rainfall data (tobe disaggregated) into memory
     * Parameters: 
     *  FP_daily: a string, storing the file path and name of daily rr data file
     *  N_STATION: the number of rainfall stations in disaggrgeation 
     *  p_rr_d: name of structure df_rr_d array
     * Return:
     *  output the number of days (rows)
     * ****************/
    // char FP_daily[]="D:/kNN_MOF_cp/data/rr_obs_daily.csv";  // key parameter
    FILE *fp_d;
    if ((fp_d=fopen(FP_daily, "r")) == NULL) {
        printf("Cannot open daily rr data file: %s\n", FP_daily);
        exit(1);
    }
    // struct df_rr_d df_rr_daily[10000];
    char *token;
    char row[MAXCHAR];
    int i, j;
    i=0;  // record the number of rows in the data file
    while (fgets(row, MAXCHAR, fp_d) != NULL)
    {
        (p_rr_d + i)->date.y = atoi(strtok(row, ","));  //df_rr_daily[i].
        (p_rr_d + i)->date.m = atoi(strtok(NULL, ","));
        (p_rr_d + i)->date.d = atoi(strtok(NULL, ","));
        (p_rr_d + i)->p_rr = (double *)malloc(N_STATION * sizeof(double));
        
        for (j=0; j < N_STATION; j++){
            token = strtok(NULL, ",");
            *((p_rr_d + i)->p_rr + j) = atof(token);
        }
        i++;
    }
    fclose(fp_d);
    return i;
}

int import_dfrr_h(
    char FP_hourly[], 
    int N_STATION,
    struct df_rr_h *p_rr_h
) {
    /**************
     * Main:
     *  import hourly rainfall observations into memory
     * Parameters: 
     *  FP_hourly: a string, storing the file path and name of hourly rr data file
     *  N_STATION: the number of rainfall stations in disaggrgeation 
     *  p_rr_h: name of structure df_rr_h array
     * Return:
     *  output the number of hourly observation days
     * ****************/
    // char FP_hourly[]="D:/kNN_MOF_cp/data/rr_obs_hourly.csv";
    FILE *fp_h;
    if ((fp_h=fopen(FP_hourly, "r")) == NULL) {
        printf("Cannot open hourly rr data file: %s\n", FP_hourly);
        exit(1);
    }
    char *token;
    char row[MAXCHAR];
    int j, h, nrow_total, ndays;
    int i = 0;

    struct df_rr_h *p_df_rr_h;  // pointer of df_rr_h; for iteration
    p_df_rr_h = p_rr_h; // initialize
    while (fgets(row, MAXCHAR, fp_h) != NULL) {
        if (i%24 == 0) {
            /* %: remainder after division (modulo division)*/
            (p_df_rr_h->date).y = atoi(strtok(row, ","));
            (p_df_rr_h->date).m = atoi(strtok(NULL, ","));
            (p_df_rr_h->date).d = atoi(strtok(NULL, ","));
            p_df_rr_h->rr_h = calloc(N_STATION, sizeof(double) * 24);  // allocate memory (stack)
        } else {
            // just skip the date (y, m, d)
            token = strtok(row, ","); token = strtok(NULL, ","); token = strtok(NULL, ",");
        }
        h = atoi(strtok(NULL, ","));
        for (j=0; j < N_STATION; j++){
            token = strtok(NULL, ",");
            *(*(p_df_rr_h->rr_h + j) + h) = atof(token);
        }
        if (i%24 == 23) {
            p_df_rr_h++;
        }
        i++;
    }
    fclose(fp_h);
    nrow_total = i;  // the total number of row in the data file
    ndays = p_df_rr_h - p_rr_h;  // the exact size of struct p_rr_h array
    /**** aggregate the hourly rr into daily scale ****/
    for (p_df_rr_h = p_rr_h; p_df_rr_h<p_rr_h + ndays; p_df_rr_h++) {
        p_df_rr_h->rr_d = (double *)malloc(N_STATION*sizeof(double)); // allocate memory (stack)
        for (j=0;j<N_STATION;j++) {
            *(p_df_rr_h->rr_d+j) = 0;
            for (h=0;h<24;h++) {
                *(p_df_rr_h->rr_d+j) += p_df_rr_h->rr_h[j][h];
            }
        }
    }
    return ndays; // the last is null
}

int import_df_cp(
    char fname[],
    struct df_cp *p_df_cp
) {
    /*********************
    * Main function:
    *     import the circulation pattern classification results
    * Parameters:
    *     fname: the file path, together with the file name of CP data
    * Return:
    *     bring back struct array of cp data to main() function;
    *     the return value of the function: the number of rows in the data file
    *********************/
    FILE *fp_cp;
    char row[MAXCHAR];
    char *token;
    int j = 0;  // from the first row 
    if ((fp_cp=fopen(fname, "r")) == NULL) {
        printf("Cannot open cp data file: %s\n", fname);
        exit(1);
    }
    while (fgets(row, MAXCHAR, fp_cp) != NULL)
    {
        // the fgets() function comes from <stdbool.h>
        // Reads characters from stream and stores them as a C string
        token = strtok(row, ",");  
        p_df_cp[j].date.y = atoi(token);
        p_df_cp[j].date.m = atoi(strtok(NULL, ","));
        p_df_cp[j].date.d = atoi(strtok(NULL, ","));
        p_df_cp[j].cp = atoi(strtok(NULL, ","));
        j++;
    } 
    fclose(fp_cp);
    return j;  // the number of rows; the last row is null
}

void Write_df_rr_h(
    struct df_rr_h *p_out,
    struct Para_global *p_gp,
    FILE *p_FP_OUT
){
    /**************
     * Description:
     *      write the disaggregated results into output file (.csv)
     * Parameters:
     *      p_gp: 
     *      p_FP_OUT: a FILE pointer, pointing to the output file
     * 
     * ************/
    int j,h;
    for (h=0;h<24;h++) {
        fprintf(
            p_FP_OUT,
            "%d,%d,%d,%d,%.3f", 
            p_out->date.y, p_out->date.m, p_out->date.d, h, 
            p_out->rr_h[0][h]
        ); // print the date and time (y, m, d, h), together with the value from first rr gauge (0)
        
        for (j=1;j<p_gp->N_STATION;j++){
            fprintf(
                p_FP_OUT,
                ",%.3f", p_out->rr_h[j][h]
            );
        }
        fprintf(p_FP_OUT, "\n"); // print "\n" (newline) after one row
    }
    // printf("%d-%d-%d: Done\n", p_out->date.y, p_out->date.m, p_out->date.d); // print to screen (command line)
}

