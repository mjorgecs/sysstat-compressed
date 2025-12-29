#include "utils.h"


void read_cpu_stats(struct stats_cpu *scc, struct stats_cpu *scp, int *nr_cpu,
                    void *m, int first_record, long *deltas) {
    
    if (first_record) {
        *nr_cpu = *(int*)m;
        m += sizeof(int);
        // only read the all cpu stats
        scc = (struct stats_cpu *)malloc(sizeof(struct stats_cpu));
        scp = (struct stats_cpu *)malloc(sizeof(struct stats_cpu));
        memcpy(scc, m, sizeof(struct stats_cpu));
        m += sizeof(struct stats_cpu);
        return;
    }

    struct stats_cpu *prev = scp;
    
    // Read CPU stats (undo deltas)
    memcpy(deltas, m, sizeof(long) * N_CPU);
    m += sizeof(long) * N_CPU;

    scc->cpu_user = (unsigned long long)(deltas[0] + prev->cpu_user);
    scc->cpu_nice = (unsigned long long)(deltas[1] + prev->cpu_nice);
    scc->cpu_sys = (unsigned long long)(deltas[2] + prev->cpu_sys);
    scc->cpu_idle = (unsigned long long)(deltas[3] + prev->cpu_idle);
    scc->cpu_iowait = (unsigned long long)(deltas[4] + prev->cpu_iowait);
    scc->cpu_steal = (unsigned long long)(deltas[5] + prev->cpu_steal);
    scc->cpu_hardirq = (unsigned long long)(deltas[6] + prev->cpu_hardirq);
    scc->cpu_softirq = (unsigned long long)(deltas[7] + prev->cpu_softirq);
    scc->cpu_guest = (unsigned long long)(deltas[8] + prev->cpu_guest);
    scc->cpu_guest_nice = (unsigned long long)(deltas[9] + prev->cpu_guest_nice);

    print_cpu_stats(scc, scp, *nr_cpu);
    
}

void write_memory_stats(struct stats_memory *smc, struct stats_memory *smp, 
                        FILE *fd, int first_record, void *m, long *deltas) {
    
    struct stats_memory *curr = smc;
    struct stats_memory *prev = smp;

    if (first_record) {
        fwrite((void*)smc, sizeof(struct stats_memory), 1, fd);
        return;
    }

    memcpy(deltas, m, sizeof(long) * N_MEMORY);
    m += sizeof(long) * N_MEMORY;

    smc->frmkb = (unsigned long long) (deltas[0] + prev->frmkb);
    smc->availablekb = (unsigned long long) (deltas[1] + prev->availablekb);
    smc->bufkb = (unsigned long long) (deltas[2] + prev->bufkb);
    smc->camkb = (unsigned long long) (deltas[3] + prev->camkb);
    smc->comkb = (unsigned long long) (deltas[4] + prev->comkb);
    smc->activekb = (unsigned long long) (deltas[5] + prev->activekb);
    smc->inactkb = (unsigned long long) (deltas[6] + prev->inactkb);
    smc->dirtykb = (unsigned long long) (deltas[7] + prev->dirtykb);
    smc->shmemkb = (unsigned long long) (deltas[8] + prev->shmemkb);

    smc->tlmkb = (unsigned long long) (deltas[9] + prev->tlmkb);
    smc->caskb = (unsigned long long) (deltas[10] + prev->caskb);
    smc->anonpgkb = (unsigned long long) (deltas[11] + prev->anonpgkb);
    smc->slabkb = (unsigned long long) (deltas[12] + prev->slabkb);
    smc->kstackkb = (unsigned long long) (deltas[13] + prev->kstackkb);
    smc->pgtblkb = (unsigned long long) (deltas[14] + prev->pgtblkb);
    smc->vmusedkb = (unsigned long long) (deltas[15] + prev->vmusedkb);

    smc->frskb = (unsigned long long) (deltas[16] + prev->frskb);
    smc->tlskb = (unsigned long long) (deltas[17] + prev->tlskb);

    for (int i = 0; i < N_MEMORY; i++) {
        fwrite((void *)&deltas[i], sizeof(long), 1, fd);
    }
    
    print_memory_stats(smc);
}

void write_paging_stats(struct stats_paging *spc, struct stats_paging *spp, FILE *fd, int first_record) {
    struct stats_paging *curr = spc;
    struct stats_paging *prev = spp;

    if (first_record) {
        fwrite((void*)spc, sizeof(struct stats_paging), 1, fd);
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
    }

    #ifdef VERBOSE
    printf("\n%-12s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s\n",
            "PAGING", "pgpgin/s", "pgpgout/s", "fault/s", "majflt/s", "pgfree/s", "pgscank/s", "pgscand/s", "pgsteal/s", "pgprom/s", "pgdem/s");    
    
    printf("%-12s  %9ld  %9ld  %9ld  %9ld  %9ld  %9ld  %9ld  %9ld  %9ld  %9ld\n",
        "delta", deltas[0], deltas[1], deltas[2], deltas[3], deltas[4], deltas[5], deltas[6], deltas[7], deltas[8], deltas[9]);
    #endif
}

void write_io_stats(struct stats_io *sic, struct stats_io *sip, FILE *fd, int first_record) {
    struct stats_io *curr = sic;
    struct stats_io *prev = sip;

    if (first_record) {
        fwrite((void*)sic, sizeof(struct stats_io), 1, fd);
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

void write_queue_stats(struct stats_queue *sqc, struct stats_queue *sqp, FILE *fd, int first_record) {

    struct stats_queue *curr = sqc;
    struct stats_queue *prev = sqp;

    if (first_record) {
        fwrite((void*)sqc, sizeof(struct stats_queue), 1, fd);
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
    }
    
    #ifdef VERBOSE
    printf("\n%-12s  %12s  %12s  %12s  %12s  %12s  %12s\n",
            "QUEUE", 
            "runq-sz", "plist-sz", "ldavg-1", "ldavg-5", "ldavg-15", "blocked");    
    
    printf("%-12s  %12ld  %12ld  %12ld  %12ld  %12ld  %12ld\n",
        "delta", deltas[0], deltas[1], deltas[2], deltas[3], deltas[4], deltas[5]);
    #endif
}

