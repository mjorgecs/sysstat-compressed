#include "utils.h"

void write_cpu_stats(struct stats_cpu *scc, struct stats_cpu *scp, int nr_cpu, FILE *fd, int first_record) {
    
    for (int i = 0; i < nr_cpu; i++) {
        struct stats_cpu *curr = &scc[i];
        struct stats_cpu *prev = &scp[i];
        
        #ifdef VERBOSE
        printf("\n%-12s  %5s  %5s  %5s  %5s  %5s  %5s  %5s  %5s  %5s  %5s\n",
                "CPU", "user", "nice", "system", "idle", "iowait", "steal", "hardirq", "softirq", "guest", "gnice");
        printf("%-12d  ", i);
        #endif

        if (first_record) {
            fwrite((void*) curr, sizeof(struct stats_cpu), 1, fd);
            #ifdef VERBOSE
            printf("%5llu  %5llu  %5llu  %5llu   %5llu  %5llu  %5llu   %5llu   %5llu  %5llu\n",
                    curr->cpu_user, curr->cpu_nice, curr->cpu_sys, curr->cpu_idle, curr->cpu_iowait, curr->cpu_steal, 
                    curr->cpu_hardirq, curr->cpu_softirq, curr->cpu_guest, curr->cpu_guest_nice);
            #endif
            continue;
        }
        
        long deltas[N_CPU] = {
        (long)(curr->cpu_user - prev->cpu_user),
        (long)(curr->cpu_nice - prev->cpu_nice),
        (long)(curr->cpu_sys - prev->cpu_sys),
        (long)(curr->cpu_idle - prev->cpu_idle),
        (long)(curr->cpu_iowait - prev->cpu_iowait),
        (long)(curr->cpu_steal - prev->cpu_steal),
        (long)(curr->cpu_hardirq - prev->cpu_hardirq),
        (long)(curr->cpu_softirq - prev->cpu_softirq),
        (long)(curr->cpu_guest - prev->cpu_guest),
        (long)(curr->cpu_guest_nice - prev->cpu_guest_nice)
        };

        for (int j = 0; j < N_CPU; j++) {
            fwrite((void *)&deltas[j], sizeof(long), 1, fd);
            #ifdef VERBOSE
            printf("%5ld  ", deltas[j]);
            #endif
        }
        #ifdef VERBOSE
        printf("\n");
        #endif        
    }
}

void write_memory_stats(struct stats_memory *smc, struct stats_memory *smp, FILE *fd, int first_record) {
    
    struct stats_memory *curr = smc;
    struct stats_memory *prev = smp;

    #ifdef VERBOSE
    printf("\n%-12s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s\n",
           "MEMORY", "frmkb", "availablekb", "bufkb", "camkb", "comkb", "activekb", "inactkb", "dirtykb", "shmemkb",
           "tlmkb", "caskb", "anonpgkb", "slabkb", "kstackkb", "pgtblkb", "vmusedkb", "frskb", "tlskb");
    printf("%-12s  ", "");
    #endif
    if (first_record) {
        fwrite((void*)smc, sizeof(struct stats_memory), 1, fd);
        #ifdef VERBOSE
        printf("%9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu\n",
            curr->frmkb, curr->availablekb, curr->bufkb, curr->camkb, curr->comkb, curr->activekb, curr->inactkb, curr->dirtykb, curr->shmemkb,
            curr->tlmkb, curr->caskb, curr->anonpgkb, curr->slabkb, curr->kstackkb, curr->pgtblkb, curr->vmusedkb,
            curr->frskb, curr->tlskb);
        #endif
        return;
    }

    long deltas[N_MEMORY] = {
        (long)(curr->frmkb - prev->frmkb),
        (long)(curr->availablekb - prev->availablekb),
        (long)(curr->bufkb - prev->bufkb),
        (long)(curr->camkb - prev->camkb),
        (long)(curr->comkb - prev->comkb),
        (long)(curr->activekb - prev->activekb),
        (long)(curr->inactkb - prev->inactkb),
        (long)(curr->dirtykb - prev->dirtykb),
        (long)(curr->shmemkb - prev->shmemkb),

        (long)(curr->tlmkb - prev->tlmkb),
        (long)(curr->caskb - prev->caskb),
        (long)(curr->anonpgkb - prev->anonpgkb),
        (long)(curr->slabkb - prev->slabkb),
        (long)(curr->kstackkb - prev->kstackkb),
        (long)(curr->pgtblkb - prev->pgtblkb),
        (long)(curr->vmusedkb - prev->vmusedkb),

        (long)(curr->frskb - prev->frskb),
        (long)(curr->tlskb - prev->tlskb)
    };        

    for (int i = 0; i < N_MEMORY; i++) {
        fwrite((void *)&deltas[i], sizeof(long), 1, fd);
        #ifdef VERBOSE
        printf("%9ld  ", deltas[i]);
        #endif
    }
    #ifdef VERBOSE
    printf("\n");
    #endif
}

void write_paging_stats(struct stats_paging *spc, struct stats_paging *spp, FILE *fd, int first_record) {
    struct stats_paging *curr = spc;
    struct stats_paging *prev = spp;

    #ifdef VERBOSE
    printf("\n%-12s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s\n",
            "PAGING", "pgpgin", "pgpgout", "pgfault", "pgmajfault", "pgfree", "pgscan_kswapd",
            "pgscan_direct", "pgsteal", "pgpromote", "pgdemote");
    printf("%-12s  ", "");
    #endif

    if (first_record) {
        fwrite((void*)spc, sizeof(struct stats_paging), 1, fd);
        #ifdef VERBOSE
        printf("%9lu  %9lu  %9lu  %9lu  %9lu  %9lu  %9lu  %9lu  %9lu  %9lu\n",
            curr->pgpgin, curr->pgpgout, curr->pgfault, curr->pgmajfault, curr->pgfree,
            curr->pgscan_kswapd, curr->pgscan_direct, curr->pgsteal, curr->pgpromote, curr->pgdemote);
        #endif
        return;
    }

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
        #ifdef VERBOSE
        printf("%9ld   ", deltas[i]);
        #endif
    }
    #ifdef VERBOSE
    printf("\n");
    #endif
}

void write_io_stats(struct stats_io *sic, struct stats_io *sip, FILE *fd, int first_record) {
    struct stats_io *curr = sic;
    struct stats_io *prev = sip;
    
    #ifdef VERBOSE
    printf("\n%-12s  %9s  %9s  %9s  %9s  %9s  %9s  %9s\n",
           "IO", "dk_drive", "dk_drive_rio", "dk_drive_wio", "dk_drive_dio", "dk_drive_rblk",
           "dk_drive_wblk", "dk_drive_dblk");
    printf("%-12s  ", "");
    #endif

    if (first_record) {
        fwrite((void*)sic, sizeof(struct stats_io), 1, fd);
        #ifdef VERBOSE
        printf("%9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu\n",
            curr->dk_drive, curr->dk_drive_rio, curr->dk_drive_wio, curr->dk_drive_dio,
            curr->dk_drive_rblk, curr->dk_drive_wblk, curr->dk_drive_dblk);
        #endif
        return;
    }

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
        #ifdef VERBOSE
        printf("%9ld  ", deltas[i]);
        #endif
    }
    #ifdef VERBOSE
    printf("\n");
    #endif
}

void write_queue_stats(struct stats_queue *sqc, struct stats_queue *sqp, FILE *fd, int first_record) {

    struct stats_queue *curr = sqc;
    struct stats_queue *prev = sqp;

    #ifdef VERBOSE
    printf("\n%-12s  %12s  %12s  %12s  %12s  %12s  %12s\n",
            "QUEUE", "nr_running", "nr_threads", "load_avg_1", "load_avg_5", "load_avg_15", "procs_blocked");
    printf("%-12s  ", ""); 
    #endif
    
    if (first_record) {
        fwrite((void*)sqc, sizeof(struct stats_queue), 1, fd);
        #ifdef VERBOSE
        printf("%12llu  %12llu  %12u  %12u  %12u  %12llu\n",
            curr->nr_running, curr->nr_threads, curr->load_avg_1, curr->load_avg_5,
            curr->load_avg_15, curr->procs_blocked);
        #endif
        return;
    }

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
        #ifdef VERBOSE
        printf("%12ld  ", deltas[i]);
        #endif
    }
    #ifdef VERBOSE
    printf("\n");
    #endif
}

