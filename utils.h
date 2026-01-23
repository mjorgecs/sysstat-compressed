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


void write_cpu_stats(struct stats_cpu *scc, struct stats_cpu *scp, int nr_cpu, FILE *fd, int first_record);

void write_memory_stats(struct stats_memory *smc, struct stats_memory *smp, FILE *fd, int first_record);

void write_paging_stats(struct stats_paging *spc, struct stats_paging *spp, FILE *fd, int first_record);

void write_io_stats(struct stats_io *sic, struct stats_io *sip, FILE *fd, int first_record);

void write_queue_stats(struct stats_queue *sqc, struct stats_queue *sqp, FILE *fd, int first_record);

int get_pos(struct activity *act[], unsigned int act_flag);

void read_cpu_stats(struct stats_cpu ***scc, struct stats_cpu ***scp, int nr_cpu,
                    void **m, int first_record, long *deltas, FILE *target_file);

void read_memory_stats(struct stats_memory **smc, struct stats_memory **smp, 
                        FILE *fd, int first_record, void **m, long *deltas);

void read_paging_stats(struct stats_paging **spc, struct stats_paging **spp, unsigned long long itv,
                        FILE *fd, int first_record, void **m, long *deltas);

void read_io_stats(struct stats_io **sic, struct stats_io **sip, FILE *fd, 
                    int first_record, void **m, long *deltas, unsigned long long itv);

void read_queue_stats(struct stats_queue **sqc, struct stats_queue **sqp, FILE *fd, int first_record,
                    void **m, long *deltas);

void set_activity_flags(int argc, int nr_act, char **argv, int **act_flags);

void usage(char * prog_name);

int is_selected(int act_id, int *act_flags, int nr_act);

void compress_stats(struct activity *act, int curr, int prev, unsigned int act_id, FILE *target_file, int first_record);
