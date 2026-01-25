#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../sysstat-repo/sa.h"
#include "../sysstat-repo/rd_stats.h"


// Number of fields per stats structure
#define N_QUEUE 6
#define N_IO 7
#define N_PAGING 10
#define N_MEMORY 18
#define N_CPU 10
#define N_RECORD_HDR_ULL 2


struct act_t{
    
    int nr;

    void *act[2];

    long *deltas;
};


void write_cpu_stats(struct stats_cpu *scc, struct stats_cpu *scp, int nr_cpu, FILE *fd, int first_record);

void write_memory_stats(struct stats_memory *smc, struct stats_memory *smp, FILE *fd, int first_record);

void write_paging_stats(struct stats_paging *spc, struct stats_paging *spp, FILE *fd, int first_record);

void write_io_stats(struct stats_io *sic, struct stats_io *sip, FILE *fd, int first_record);

void write_queue_stats(struct stats_queue *sqc, struct stats_queue *sqp, FILE *fd, int first_record);

int get_pos(struct activity *act[], unsigned int act_flag);

void read_cpu_stats(struct act_t **act, int curr, int prev, void **m, int first_record);

void read_memory_stats(struct act_t **act, int curr, int prev, void **m, int first_record);

void read_paging_stats(struct act_t **act, int curr, int prev, void **m, int first_record);

void read_io_stats(struct act_t **act, int curr, int prev, void **m, int first_record);

void read_queue_stats(struct act_t **act, int curr, int prev, void **m, int first_record);

void set_activity_flags(int argc, int nr_act, char **argv, int **act_flags);

void usage(char * prog_name);

int is_selected(int act_id, int *act_flags, int nr_act);

void compress_stats(struct activity *act, int curr, int prev, int nr_value, unsigned int act_id, FILE *target_file, int first_record);

void compress_record_hdr(struct record_header *curr_hdr, struct record_header *prev_hdr, FILE *fd, int first_record);

int check_dimensions(struct activity *act[], struct file_activity *fal, int *act_flags, int **final_flags, int new_act, int total_act);

void decompress_stats(struct act_t **act, int curr, int prev, void **m, int first_record, int act_id);

void decompress_record_hdr(struct record_header **curr_hdr, struct record_header *prev_hdr, void **m, unsigned int **deltas, int first_record);
