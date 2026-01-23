#include "utils.h"

void read_cpu_stats(struct stats_cpu ***scc, struct stats_cpu ***scp, int nr_cpu,
                    void **m, int first_record, long *deltas, FILE *target_file) {
    
    for (int i = 0; i < nr_cpu; i++) {

        if (first_record) {
            // only read the all cpu stats
            memcpy((void *) ((*scc)[i]), *m, sizeof(struct stats_cpu));
            (*m) += sizeof(struct stats_cpu);

            fwrite((void*) ((*scc)[i]), sizeof(struct stats_cpu), 1, target_file);
            continue;
        }
        
        // Read CPU stats (undo deltas)
        memcpy(deltas, *m, sizeof(long) * N_CPU);
        (*m) += sizeof(long) * N_CPU;

        ((*scc)[i])->cpu_user = (unsigned long long)(deltas[0] + ((*scp)[i])->cpu_user);
        ((*scc)[i])->cpu_nice = (unsigned long long)(deltas[1] + ((*scp)[i])->cpu_nice);
        ((*scc)[i])->cpu_sys = (unsigned long long)(deltas[2] + ((*scp)[i])->cpu_sys);
        ((*scc)[i])->cpu_idle = (unsigned long long)(deltas[3] + ((*scp)[i])->cpu_idle);
        ((*scc)[i])->cpu_iowait = (unsigned long long)(deltas[4] + ((*scp)[i])->cpu_iowait);
        ((*scc)[i])->cpu_steal = (unsigned long long)(deltas[5] + ((*scp)[i])->cpu_steal);
        ((*scc)[i])->cpu_hardirq = (unsigned long long)(deltas[6] + ((*scp)[i])->cpu_hardirq);
        ((*scc)[i])->cpu_softirq = (unsigned long long)(deltas[7] + ((*scp)[i])->cpu_softirq);
        ((*scc)[i])->cpu_guest = (unsigned long long)(deltas[8] + ((*scp)[i])->cpu_guest);
        ((*scc)[i])->cpu_guest_nice = (unsigned long long)(deltas[9] + ((*scp)[i])->cpu_guest_nice);    
        fwrite((void*) ((*scc)[i]), sizeof(struct stats_cpu), 1, target_file);
    }
}

void read_memory_stats(struct stats_memory **smc, struct stats_memory **smp, 
                        FILE *fd, int first_record, void **m, long *deltas) {
    
    struct stats_memory *curr = *smc;
    struct stats_memory *prev = *smp;
    if (first_record) {
        *smc = (struct stats_memory *)malloc(sizeof(struct stats_memory));
        *smp = (struct stats_memory *)malloc(sizeof(struct stats_memory));
        memcpy(*smc, *m, sizeof(struct stats_memory));
        (*m) += sizeof(struct stats_memory);
        return;
    }

    memcpy(deltas, *m, sizeof(long) * N_MEMORY);
    (*m) += sizeof(long) * N_MEMORY;

    (*smc)->frmkb = (unsigned long long) (deltas[0] + prev->frmkb);
    (*smc)->availablekb = (unsigned long long) (deltas[1] + prev->availablekb);
    (*smc)->bufkb = (unsigned long long) (deltas[2] + prev->bufkb);
    (*smc)->camkb = (unsigned long long) (deltas[3] + prev->camkb);
    (*smc)->comkb = (unsigned long long) (deltas[4] + prev->comkb);
    (*smc)->activekb = (unsigned long long) (deltas[5] + prev->activekb);
    (*smc)->inactkb = (unsigned long long) (deltas[6] + prev->inactkb);
    (*smc)->dirtykb = (unsigned long long) (deltas[7] + prev->dirtykb);
    (*smc)->shmemkb = (unsigned long long) (deltas[8] + prev->shmemkb);

    (*smc)->tlmkb = (unsigned long long) (deltas[9] + prev->tlmkb);
    (*smc)->caskb = (unsigned long long) (deltas[10] + prev->caskb);
    (*smc)->anonpgkb = (unsigned long long) (deltas[11] + prev->anonpgkb);
    (*smc)->slabkb = (unsigned long long) (deltas[12] + prev->slabkb);
    (*smc)->kstackkb = (unsigned long long) (deltas[13] + prev->kstackkb);
    (*smc)->pgtblkb = (unsigned long long) (deltas[14] + prev->pgtblkb);
    (*smc)->vmusedkb = (unsigned long long) (deltas[15] + prev->vmusedkb);

    (*smc)->frskb = (unsigned long long) (deltas[16] + prev->frskb);
    (*smc)->tlskb = (unsigned long long) (deltas[17] + prev->tlskb);
}

void read_paging_stats(struct stats_paging **spc, struct stats_paging **spp, unsigned long long itv,
                        FILE *fd, int first_record, void **m, long *deltas) {
    struct stats_paging *curr = *spc;
    struct stats_paging *prev = *spp;

    if (first_record) {
        *spc = (struct stats_paging *)malloc(sizeof(struct stats_paging));
        *spp = (struct stats_paging *)malloc(sizeof(struct stats_paging));
        memcpy(*spc, *m, sizeof(struct stats_paging));
        (*m) += sizeof(struct stats_paging);
        return;
    }

    memcpy(deltas, *m, sizeof(long) * N_PAGING);
    (*m) += sizeof(long) * N_PAGING;
    
    (*spc)->pgpgin = (unsigned long long) (deltas[0] + prev->pgpgin);
    (*spc)->pgpgout = (unsigned long long) (deltas[1] + prev->pgpgout);
    (*spc)->pgfault = (unsigned long long) (deltas[2] + prev->pgfault);
    (*spc)->pgmajfault = (unsigned long long) (deltas[3] + prev->pgmajfault);
    (*spc)->pgfree = (unsigned long long) (deltas[4] + prev->pgfree);
    (*spc)->pgscan_kswapd = (unsigned long long) (deltas[5] + prev->pgscan_kswapd);
    (*spc)->pgscan_direct = (unsigned long long) (deltas[6] + prev->pgscan_direct);
    (*spc)->pgsteal = (unsigned long long) (deltas[7] + prev->pgsteal);
    (*spc)->pgpromote = (unsigned long long) (deltas[8] + prev->pgpromote);
    (*spc)->pgdemote = (unsigned long long) (deltas[9] + prev->pgdemote);
}

void read_io_stats(struct stats_io **sic, struct stats_io **sip, FILE *fd, 
                    int first_record, void **m, long *deltas, unsigned long long itv) {
    struct stats_io *curr = *sic;
    struct stats_io *prev = *sip;

    if (first_record) {
        *sic = (struct stats_io *)malloc(sizeof(struct stats_io));
        *sip = (struct stats_io *)malloc(sizeof(struct stats_io));
        memcpy(*sic, *m, sizeof(struct stats_io));
        (*m) += sizeof(struct stats_io);
        return;
    }

    memcpy(deltas, *m, sizeof(long) * N_IO);
    (*m) += sizeof(long) * N_IO;

    (*sic)->dk_drive  = (unsigned long long) (deltas[0] + prev->dk_drive);
    (*sic)->dk_drive_rio  = (unsigned long long) (deltas[1] + prev->dk_drive_rio);
    (*sic)->dk_drive_wio  = (unsigned long long) (deltas[2] + prev->dk_drive_wio);
    (*sic)->dk_drive_dio  = (unsigned long long) (deltas[3] + prev->dk_drive_dio);
    (*sic)->dk_drive_rblk  = (unsigned long long) (deltas[4] + prev->dk_drive_rblk);
    (*sic)->dk_drive_wblk  = (unsigned long long) (deltas[5] + prev->dk_drive_wblk);
    (*sic)->dk_drive_dblk  = (unsigned long long) (deltas[6] + prev->dk_drive_dblk);
}

void read_queue_stats(struct stats_queue **sqc, struct stats_queue **sqp, FILE *fd, int first_record,
                    void **m, long *deltas) {

    struct stats_queue *curr = *sqc;
    struct stats_queue *prev = *sqp;
    if (first_record) {
        *sqc = (struct stats_queue *)malloc(sizeof(struct stats_queue));
        *sqp = (struct stats_queue *)malloc(sizeof(struct stats_queue));
        memcpy(*sqc, *m, sizeof(struct stats_queue));
        (*m) += sizeof(struct stats_queue);
        return;
    }

    memcpy(deltas, *m, sizeof(long) * N_QUEUE);
    (*m) += sizeof(long) * N_QUEUE;

    (*sqc)->nr_running = (unsigned long long) (deltas[0] + prev->nr_running);
    (*sqc)->nr_threads = (unsigned long long) (deltas[1] + prev->nr_threads);
    (*sqc)->load_avg_1 = (unsigned long long) (deltas[2] + prev->load_avg_1);
    (*sqc)->load_avg_5 = (unsigned long long) (deltas[3] + prev->load_avg_5);
    (*sqc)->load_avg_15 = (unsigned long long) (deltas[4] + prev->load_avg_15);
    (*sqc)->procs_blocked = (unsigned long long) (deltas[5] + prev->procs_blocked);
}

