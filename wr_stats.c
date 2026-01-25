#include "utils.h"
#define DEBUG

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
        
        __comp_t deltas[N_CPU] = {
        (__comp_t)(curr->cpu_user - prev->cpu_user),
        (__comp_t)(curr->cpu_nice - prev->cpu_nice),
        (__comp_t)(curr->cpu_sys - prev->cpu_sys),
        (__comp_t)(curr->cpu_idle - prev->cpu_idle),
        (__comp_t)(curr->cpu_iowait - prev->cpu_iowait),
        (__comp_t)(curr->cpu_steal - prev->cpu_steal),
        (__comp_t)(curr->cpu_hardirq - prev->cpu_hardirq),
        (__comp_t)(curr->cpu_softirq - prev->cpu_softirq),
        (__comp_t)(curr->cpu_guest - prev->cpu_guest),
        (__comp_t)(curr->cpu_guest_nice - prev->cpu_guest_nice)
        };

        for (int j = 0; j < N_CPU; j++) {
            fwrite((void *)&deltas[j], sizeof(__comp_t), 1, fd);
            #ifdef VERBOSE
            printf(COMP_FLAG_5(deltas[j]), COMP_ARG(deltas[j]));
            printf("  ");
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

    __comp_t deltas[N_MEMORY] = {
        (__comp_t)(curr->frmkb - prev->frmkb),
        (__comp_t)(curr->availablekb - prev->availablekb),
        (__comp_t)(curr->bufkb - prev->bufkb),
        (__comp_t)(curr->camkb - prev->camkb),
        (__comp_t)(curr->comkb - prev->comkb),
        (__comp_t)(curr->activekb - prev->activekb),
        (__comp_t)(curr->inactkb - prev->inactkb),
        (__comp_t)(curr->dirtykb - prev->dirtykb),
        (__comp_t)(curr->shmemkb - prev->shmemkb),

        (__comp_t)(curr->tlmkb - prev->tlmkb),
        (__comp_t)(curr->caskb - prev->caskb),
        (__comp_t)(curr->anonpgkb - prev->anonpgkb),
        (__comp_t)(curr->slabkb - prev->slabkb),
        (__comp_t)(curr->kstackkb - prev->kstackkb),
        (__comp_t)(curr->pgtblkb - prev->pgtblkb),
        (__comp_t)(curr->vmusedkb - prev->vmusedkb),

        (__comp_t)(curr->frskb - prev->frskb),
        (__comp_t)(curr->tlskb - prev->tlskb)
    };        

    for (int i = 0; i < N_MEMORY; i++) {
        fwrite((void *)&deltas[i], sizeof(__comp_t), 1, fd);
        #ifdef VERBOSE
        printf(COMP_FLAG_9(deltas[i]), COMP_ARG(deltas[i]));
        printf("  ");
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

    __comp_t deltas[N_PAGING] = {
        (__comp_t) (curr->pgpgin - prev->pgpgin),
        (__comp_t) (curr->pgpgout - prev->pgpgout),
        (__comp_t) (curr->pgfault - prev->pgfault),
        (__comp_t) (curr->pgmajfault - prev->pgmajfault),
        (__comp_t) (curr->pgfree - prev->pgfree),
        (__comp_t) (curr->pgscan_kswapd - prev->pgscan_kswapd),
        (__comp_t) (curr->pgscan_direct - prev->pgscan_direct),
        (__comp_t) (curr->pgsteal - prev->pgsteal),
        (__comp_t) (curr->pgpromote - prev->pgpromote),
        (__comp_t) (curr->pgdemote - prev->pgdemote)
    };

    for (int i = 0; i < N_PAGING; i++) {
        fwrite((void *)&deltas[i], sizeof(__comp_t), 1, fd);
        #ifdef VERBOSE
        printf(COMP_FLAG_9(deltas[i]), COMP_ARG(deltas[i]));
        printf("  ");
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

    __comp_t deltas[N_IO] = {
        (__comp_t) (curr->dk_drive - prev->dk_drive),
        (__comp_t) (curr->dk_drive_rio - prev->dk_drive_rio),
        (__comp_t) (curr->dk_drive_wio - prev->dk_drive_wio),
        (__comp_t) (curr->dk_drive_dio - prev->dk_drive_dio),
        (__comp_t) (curr->dk_drive_rblk - prev->dk_drive_rblk),
        (__comp_t) (curr->dk_drive_wblk - prev->dk_drive_wblk),
        (__comp_t) (curr->dk_drive_dblk - prev->dk_drive_dblk)
    };

    for (int i = 0; i < N_IO; i++) {
        fwrite((void *)&deltas[i], sizeof(__comp_t), 1, fd);
        #ifdef VERBOSE
        printf(COMP_FLAG_9(deltas[i]), COMP_ARG(deltas[i]));
        printf("  ");
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

    __comp_t deltas[N_QUEUE] = {
        (__comp_t) (curr->nr_running - prev->nr_running),
        (__comp_t) (curr->nr_threads - prev->nr_threads),
        (__comp_t) (curr->load_avg_1 - prev->load_avg_1),
        (__comp_t) (curr->load_avg_5 - prev->load_avg_5),
        (__comp_t) (curr->load_avg_15 - prev->load_avg_15),
        (__comp_t) (curr->procs_blocked - prev->procs_blocked)
    };

    for (int i = 0; i < N_QUEUE; i++) {
        fwrite((void *)&deltas[i], sizeof(__comp_t), 1, fd);
        #ifdef VERBOSE
        printf(COMP_FLAG_12(deltas[i]), COMP_ARG(deltas[i]));
        printf("  ");
        #endif
    }
    #ifdef VERBOSE
    printf("\n");
    #endif
}

void compress_record_hdr(struct record_header *curr_hdr, struct record_header *prev_hdr, FILE *fd, int first_record) {
    struct record_header *curr = curr_hdr;
    struct record_header *prev = prev_hdr;

    #ifdef VERBOSE
    printf("\n%-12s  %9s  %9s  %9s  %9s  %9s  %9s  %9s\n",
            "RECORD HEADER", "uptime_cs", "ust_time", "extra_next", "record_type",
            "hour", "minute", "second");
    printf("%-12s  ", "");
    #endif

    if (first_record) {
        fwrite((void*)curr, RECORD_HEADER_SIZE, 1, fd);
        #ifdef VERBOSE
        printf("%9llu  %9llu  ", curr->uptime_cs, curr->ust_time);
        #endif
    } else {
        unsigned int deltas[N_RECORD_HDR_ULL] = {
            (unsigned int) (curr->uptime_cs - prev->uptime_cs),
            (unsigned int) (curr->ust_time - prev->ust_time),
        };
    
        for (int i = 0; i < N_RECORD_HDR_ULL; i++) {
            fwrite((void *)&deltas[i], sizeof(unsigned int), 1, fd);
            #ifdef VERBOSE
            printf("%9u  ", deltas[i]);
            #endif
        }
    
        fwrite((void *)&curr->extra_next, sizeof(unsigned int), 1, fd);
        fwrite((void *)&curr->record_type, sizeof(unsigned char), 1, fd);
        fwrite((void *)&curr->hour, sizeof(unsigned char), 1, fd);
        fwrite((void *)&curr->minute, sizeof(unsigned char), 1, fd);
        fwrite((void *)&curr->second, sizeof(unsigned char), 1, fd);
    }

    #ifdef VERBOSE
    printf("%9u  %9u  %9u  %9u  %9u\n",curr->extra_next, curr->record_type, curr->hour, curr->minute, curr->second);
    #endif
}