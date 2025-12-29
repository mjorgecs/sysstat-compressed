#include "../sysstat-repo/sa.h"
#include "../sysstat-repo/rd_stats.h"



#define N_QUEUE 6
#define N_IO 7
#define N_PAGING 10
#define N_MEMORY 18
#define N_CPU 10

/*
 * Macros used to display statistics values.
 *
 */
/* With S_VALUE macro, the interval of time (@p) is given in 1/100th of a second */
#define S_VALUE(m,n,p)		(((double) ((n) - (m))) / (p) * 100)
/* Define SP_VALUE() to normalize to % */
#define SP_VALUE(m,n,p)		(((double) ((n) - (m))) / (p) * 100)


void print_cpu_stats(struct stats_cpu *scc, struct stats_cpu *scp, int nr_cpu);

void print_memory_stats(struct stats_memory *smc);

void print_paging_stats(struct stats_paging *spc, struct stats_paging *spp, unsigned long long itv);

void print_io_stats(struct stats_io *sic, struct stats_io *sip, unsigned long long itv);

void print_queue_stats(struct stats_queue *sqc);

void write_cpu_stats(struct stats_cpu *scc, struct stats_cpu *scp, int nr_cpu, FILE *fd, int first_record);

void write_memory_stats(struct stats_memory *smc, struct stats_memory *smp, FILE *fd, int first_record);

void write_paging_stats(struct stats_paging *spc, struct stats_paging *spp, FILE *fd, int first_record);

void write_io_stats(struct stats_io *sic, struct stats_io *sip, FILE *fd, int first_record);

void write_queue_stats(struct stats_queue *sqc, struct stats_queue *sqp, FILE *fd, int first_record);

int get_pos(struct activity *act[], unsigned int act_flag);
