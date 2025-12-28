#include "pr_stats.h"

#include "pr_stats.h"

#define N_QUEUE 6
#define N_IO 7
#define N_PAGING 10
#define N_MEMORY 18
#define N_CPU 10

void print_cpu_stats(struct stats_cpu *scc, struct stats_cpu *scp, int nr_cpu) {
    
    printf("\n%-12s  %5s  %5s  %5s  %5s  %5s  %5s  %5s  %5s  %5s  %5s\n",
            "CPU", "%usr", "%nice", "%system", "%iowait", "%steal", "%irq", "%soft", "%guest", "%gnice", "%idle");

    for (int i = 0; i < nr_cpu; i++) {
        struct stats_cpu *curr = &scc[i];
        struct stats_cpu *prev = &scp[i];
        
        // Calculate deltas
        unsigned long long tot_jiffies = 0;
        unsigned long long tot_jiffies_p = 0;
        
        // Current totals
        tot_jiffies = curr->cpu_user + curr->cpu_nice + curr->cpu_sys +
                      curr->cpu_idle + curr->cpu_iowait + curr->cpu_hardirq +
                      curr->cpu_softirq + curr->cpu_steal;

        // Previous totals
        tot_jiffies_p = prev->cpu_user + prev->cpu_nice + prev->cpu_sys +
                        prev->cpu_idle + prev->cpu_iowait + prev->cpu_hardirq +
                        prev->cpu_softirq + prev->cpu_steal;
        
        // Calculate difference
        unsigned long long diff_total = tot_jiffies - tot_jiffies_p;
        
        if (diff_total == 0) {
            diff_total = 1;  // Avoid division by zero
        }
        
        // Calculate percentages
        double pc_user = (double)(curr->cpu_user - prev->cpu_user) * 100.0 / diff_total;
        double pc_nice = (double)(curr->cpu_nice - prev->cpu_nice) * 100.0 / diff_total;
        double pc_sys = (double)(curr->cpu_sys - prev->cpu_sys) * 100.0 / diff_total;
        double pc_idle = (double)(curr->cpu_idle - prev->cpu_idle) * 100.0 / diff_total;
        double pc_iowait = (double)(curr->cpu_iowait - prev->cpu_iowait) * 100.0 / diff_total;
        double pc_steal = (double)(curr->cpu_steal - prev->cpu_steal) * 100.0 / diff_total;
        double pc_irq = (double)(curr->cpu_hardirq - prev->cpu_hardirq) * 100.0 / diff_total;
        double pc_soft = (double)(curr->cpu_softirq - prev->cpu_softirq) * 100.0 / diff_total;
        double pc_guest = (double)(curr->cpu_guest - prev->cpu_guest) * 100.0 / diff_total;
        double pc_gnice = (double)(curr->cpu_guest_nice - prev->cpu_guest_nice) * 100.0 / diff_total;
        
        // Print CPU name (all for first, then individual CPUs)
        if (i == 0) {
            printf("%-12s", "all");
            printf("  %5.2f  %5.2f  %5.2f  %5.2f   %5.2f  %5.2f  %5.2f   %5.2f   %5.2f  %5.2f\n",
                    pc_user, pc_nice, pc_sys, pc_iowait, pc_steal, pc_irq, pc_soft, pc_guest, pc_gnice, pc_idle);

        } /*else {
            printf("%-12d", i - 1);
            printf("  %5.2f  %5.2f  %5.2f  %5.2f   %5.2f  %5.2f  %5.2f   %5.2f   %5.2f  %5.2f\n",
            pc_user, pc_nice, pc_sys, pc_iowait, pc_steal, 
            pc_irq, pc_soft, pc_guest, pc_gnice, pc_idle);
        }*/
        
    }
}

void print_memory_stats(struct stats_memory *smc, struct stats_memory *smp, FILE *fd) {
    
    struct stats_memory *curr = smc;
    struct stats_memory *prev = smp;

    long deltas[N_MEMORY] = {
        (long) (curr->frmkb - prev->frmkb),
        (long) (curr->availablekb - prev->availablekb),
        (long) (curr->bufkb - prev->bufkb),
        (long) (curr->camkb - prev->camkb),
        (long) (curr->comkb - prev->comkb),
        (long) (curr->activekb - prev->activekb),
        (long) (curr->inactkb - prev->inactkb),
        (long) (curr->dirtykb - prev->dirtykb),
        (long) (curr->shmemkb - prev->shmemkb),

        (long) (curr->tlmkb - prev->tlmkb),
        (long) (curr->caskb - prev->caskb),
        (long) (curr->anonpgkb - prev->anonpgkb),
        (long) (curr->slabkb - prev->slabkb),
        (long) (curr->kstackkb - prev->kstackkb),
        (long) (curr->pgtblkb - prev->pgtblkb),
        (long) (curr->vmusedkb - prev->vmusedkb),

        (long) (curr->frskb - prev->frskb),
        (long) (curr->tlskb - prev->tlskb)
    };        

    for (int i = 0; i < N_MEMORY; i++) {
        fwrite((void *)&deltas[i], sizeof(long), 1, fd);
    }
    
    #ifdef VERBOSE
    /* Print deltas */
    printf("\n%-12s  %12s  %12s  %12s  %12s  %12s  %12s  %12s  %12s  %12s\n",
           "MEMORY", 
           "kbmemfree", "kbavail", "kbbuffers", "kbcached", "kbcommit", "kbactive", "kbinact", "kbdirty", "kbshmem",
           "kbmemtotal", "kbcas", "kbanonpg", "kbslab", "kbkstack", "kbpgtbl", "kbvmused");

    printf("%-12s  %12ld  %12ld  %12ld  %12ld  %12ld  %12ld  %12ld  %12ld  %12ld\n",
            "delta", deltas[0], deltas[1], deltas[2], deltas[3], deltas[4], deltas[5], deltas[6], deltas[7], deltas[8]);

    /* Print remaining deltas*/
    printf("\n%-12s  %12s  %12s  %12s  %12s  %12s  %12s  %12s\n",
           "MEMORY V2", 
           "kbmemtotal", "kbcas", "kbanonpg", "kbslab", "kbkstack", "kbpgtbl", "kbvmused");

    printf("%-12s  %12ld  %12ld  %12ld  %12ld  %12ld  %12ld  %12ld\n",
            "delta", deltas[9], deltas[10], deltas[11], deltas[12], deltas[13], deltas[14], deltas[15]);

    /* Print swap memory deltas */
    printf("\n%-12s  %12s  %12s\n",
           "SWAP MEMORY", 
           "kbswpfree", "kbswptotal");

    printf("%-12s  %12ld  %12ld\n",
            "delta", deltas[16], deltas[17]);
    #endif
}

void print_paging_stats(struct stats_paging *spc, struct stats_paging *spp, FILE *fd) {
    struct stats_paging *curr = spc;
    struct stats_paging *prev = spp;

    long deltas[N_PAGING] = {
        (long) (curr->pgpgin - prev->pgpgin),
        (long) (curr->pgpgout - prev->pgpgout),
        (long) (curr->pgfault - prev->pgfault),
        (long) (curr->pgmajfault - prev->pgmajfault),
        (long) (curr->pgfree - prev->pgfree),
        (long) (curr->pgscan_kswapd - prev->pgscan_kswapd),
        (long) (curr->pgscan_direct - prev->pgscan_direct),
        (long) (curr->pgsteal - prev->pgsteal),
        (long) (curr->pgpromote - prev->pgpromote),
        (long) (curr->pgdemote - prev->pgdemote)
    };

    for (int i = 0; i < N_PAGING; i++) {
        fwrite((void *)&deltas[i], sizeof(long), 1, fd);
    }

    #ifdef VERBOSE
    printf("\n%-12s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s\n",
            "PAGING", "pgpgin/s", "pgpgout/s", "fault/s", "majflt/s", "pgfree/s", "pgscank/s", "pgscand/s", "pgsteal/s", "pgprom/s", "pgdem/s");    
    
    printf("%-12s  %9ld  %9ld  %9ld  %9ld  %9ld  %9ld  %9ld  %9ld  %9ld  %9ld\n",
        "delta", deltas[0], deltas[1], deltas[2], deltas[3], deltas[4], deltas[5], deltas[6], deltas[7], deltas[8], deltas[9]);
    #endif
}

void write_io_stats(struct stats_io *sic, struct stats_io *sip, FILE *fd) {
    struct stats_io *curr = sic;
    struct stats_io *prev = sip;

    long deltas[N_IO] = {
        (long) (curr->dk_drive - prev->dk_drive),
        (long) (curr->dk_drive_rio - prev->dk_drive_rio),
        (long) (curr->dk_drive_wio - prev->dk_drive_wio),
        (long) (curr->dk_drive_dio - prev->dk_drive_dio),
        (long) (curr->dk_drive_rblk - prev->dk_drive_rblk),
        (long) (curr->dk_drive_wblk - prev->dk_drive_wblk),
        (long) (curr->dk_drive_dblk - prev->dk_drive_dblk)
    };

    for (int i = 0; i < N_IO; i++) {
        fwrite((void *)&deltas[i], sizeof(long), 1, fd);
    }

    /* Print io deltas */
    #ifdef VERBOSE
    printf("\n%-12s  %9s  %9s  %9s  %9s  %9s  %9s  %9s\n",
           "IO", 
           "tps", "rtps", "wtps", "dtps", "bread/s", "bwrtn/s", "bdscd/s");

    printf("%-12s  %9ld  %9ld  %9ld  %9ld  %9ld  %9ld  %9ld\n",
            "delta", deltas[0], deltas[1], deltas[2], deltas[3], deltas[4], deltas[5], deltas[6]);
    #endif
}

void write_queue_stats(struct stats_queue *sqc, struct stats_queue *sqp, FILE *fd) {

    struct stats_queue *curr = sqc;
    struct stats_queue *prev = sqp;

    long deltas[N_QUEUE] = {
        (long) (curr->nr_running - prev->nr_running),
        (long) (curr->nr_threads - prev->nr_threads),
        (long) (curr->load_avg_1 - prev->load_avg_1),
        (long) (curr->load_avg_5 - prev->load_avg_5),
        (long) (curr->load_avg_15 - prev->load_avg_15),
        (long) (curr->procs_blocked - prev->procs_blocked)
    };

    for (int i = 0; i < N_QUEUE; i++) {
        fwrite((void *)&deltas[i], sizeof(long), 1, fd);
    }
    
    #ifdef VERBOSE
    printf("\n%-12s  %12s  %12s  %12s  %12s  %12s  %12s\n",
            "QUEUE", 
            "runq-sz", "plist-sz", "ldavg-1", "ldavg-5", "ldavg-15", "blocked");    
    
    printf("%-12s  %12ld  %12ld  %12ld  %12ld  %12ld  %12ld\n",
        "delta", deltas[0], deltas[1], deltas[2], deltas[3], deltas[4], deltas[5]);
    #endif
}

