#include "utils.h"

void read_cpu_stats(struct act_t **act, int curr, int prev, void **m, int first_record) {

    struct stats_cpu *ca = (struct stats_cpu *) (*act)->act[curr];
    struct stats_cpu *pa = (struct stats_cpu *) (*act)->act[prev];
    __comp_t **deltas = &((*act)->deltas);
    
    if (first_record) {
        *deltas = (__comp_t *)malloc(sizeof(__comp_t) * N_CPU);
    }
    
    for (int i = 0; i < (*act)->nr; i++) {
        #ifdef VERBOSE
        printf("\n%-12s  %5s  %5s  %5s  %5s  %5s  %5s  %5s  %5s  %5s  %5s\n",
                "CPU", "user", "nice", "system", "idle", "iowait", "steal", "hardirq", "softirq", "guest", "gnice");
        printf("%-12d  ", i);
        #endif

        if (!first_record) {
            // Read CPU stats (undo deltas)
            memcpy(*deltas, *m, sizeof(__comp_t) * N_CPU);
            (*m) += sizeof(__comp_t) * N_CPU;
    
            ca[i].cpu_user = (unsigned long long)((*deltas)[0] + pa[i].cpu_user);
            ca[i].cpu_nice = (unsigned long long)((*deltas)[1] + pa[i].cpu_nice);
            ca[i].cpu_sys = (unsigned long long)((*deltas)[2] + pa[i].cpu_sys);
            ca[i].cpu_idle = (unsigned long long)((*deltas)[3] + pa[i].cpu_idle);
            ca[i].cpu_iowait = (unsigned long long)((*deltas)[4] + pa[i].cpu_iowait);
            ca[i].cpu_steal = (unsigned long long)((*deltas)[5] + pa[i].cpu_steal);
            ca[i].cpu_hardirq = (unsigned long long)((*deltas)[6] + pa[i].cpu_hardirq);
            ca[i].cpu_softirq = (unsigned long long)((*deltas)[7] + pa[i].cpu_softirq);
            ca[i].cpu_guest = (unsigned long long)((*deltas)[8] + pa[i].cpu_guest);
            ca[i].cpu_guest_nice = (unsigned long long)((*deltas)[9] + pa[i].cpu_guest_nice);    
        }

        #ifdef VERBOSE
        printf("%5llu  %5llu  %5llu  %5llu   %5llu  %5llu  %5llu   %5llu   %5llu  %5llu\n",
                ca[i].cpu_user, ca[i].cpu_nice, ca[i].cpu_sys, ca[i].cpu_idle, ca[i].cpu_iowait, ca[i].cpu_steal, 
                ca[i].cpu_hardirq, ca[i].cpu_softirq, ca[i].cpu_guest, ca[i].cpu_guest_nice);
        #endif
    }
}

void read_memory_stats(struct act_t **act, int curr, int prev, void **m, int first_record) {
    
    struct stats_memory *ca = (struct stats_memory *) (*act)->act[curr];
    struct stats_memory *pa = (struct stats_memory *) (*act)->act[prev];
    __comp_t **deltas = &((*act)->deltas);
    
    if (first_record) {
        *deltas = (__comp_t *)malloc(sizeof(__comp_t) * N_MEMORY);
    }

    for (int i = 0; i < (*act)->nr; i++) {
        #ifdef VERBOSE
        printf("\n%-12s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s\n",
                "MEMORY", "frmkb", "availablekb", "bufkb", "camkb", "comkb", "activekb", "inactkb", "dirtykb", "shmemkb",
                "tlmkb", "caskb", "anonpgkb", "slabkb", "kstackkb", "pgtblkb", "vmusedkb", "frskb", "tlskb");
        printf("%-12s  ", "");
        #endif

        if (!first_record) {
            // Read Memory stats (undo deltas)
            memcpy(*deltas, *m, sizeof(__comp_t) * N_MEMORY);
            (*m) += sizeof(__comp_t) * N_MEMORY;
    
            ca[i].frmkb = (unsigned long long) ((*deltas)[0] + pa[i].frmkb);
            ca[i].availablekb = (unsigned long long) ((*deltas)[1] + pa[i].availablekb);
            ca[i].bufkb = (unsigned long long) ((*deltas)[2] + pa[i].bufkb);
            ca[i].camkb = (unsigned long long) ((*deltas)[3] + pa[i].camkb);
            ca[i].comkb = (unsigned long long) ((*deltas)[4] + pa[i].comkb);
            ca[i].activekb = (unsigned long long) ((*deltas)[5] + pa[i].activekb);
            ca[i].inactkb = (unsigned long long) ((*deltas)[6] + pa[i].inactkb);
            ca[i].dirtykb = (unsigned long long) ((*deltas)[7] + pa[i].dirtykb);
            ca[i].shmemkb = (unsigned long long) ((*deltas)[8] + pa[i].shmemkb);
    
            ca[i].tlmkb = (unsigned long long) ((*deltas)[9] + pa[i].tlmkb);
            ca[i].caskb = (unsigned long long) ((*deltas)[10] + pa[i].caskb);
            ca[i].anonpgkb = (unsigned long long) ((*deltas)[11] + pa[i].anonpgkb);
            ca[i].slabkb = (unsigned long long) ((*deltas)[12] + pa[i].slabkb);
            ca[i].kstackkb = (unsigned long long) ((*deltas)[13] + pa[i].kstackkb);
            ca[i].pgtblkb = (unsigned long long) ((*deltas)[14] + pa[i].pgtblkb);
            ca[i].vmusedkb = (unsigned long long) ((*deltas)[15] + pa[i].vmusedkb);
    
            ca[i].frskb = (unsigned long long) ((*deltas)[16] + pa[i].frskb);
            ca[i].tlskb = (unsigned long long) ((*deltas)[17] + pa[i].tlskb);
        }

        #ifdef VERBOSE
        printf("%9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu\n",
                ca[i].frmkb, ca[i].availablekb, ca[i].bufkb, ca[i].camkb, ca[i].comkb, ca[i].activekb, ca[i].inactkb, ca[i].dirtykb, ca[i].shmemkb,
                ca[i].tlmkb, ca[i].caskb, ca[i].anonpgkb, ca[i].slabkb, ca[i].kstackkb, ca[i].pgtblkb, ca[i].vmusedkb,
                ca[i].frskb, ca[i].tlskb);
        #endif
    }
}

void read_paging_stats(struct act_t **act, int curr, int prev, void **m, int first_record) {

    struct stats_paging *ca = (struct stats_paging *) (*act)->act[curr];
    struct stats_paging *pa = (struct stats_paging *) (*act)->act[prev];
    __comp_t **deltas = &((*act)->deltas);

    if (first_record) {
        *deltas = (__comp_t *)malloc(sizeof(__comp_t) * N_PAGING);
    }

    for (int i = 0; i < (*act)->nr; i++) {
        #ifdef VERBOSE
        printf("\n%-12s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s  %9s\n",
                "PAGING", "pgpgin", "pgpgout", "pgfault", "pgmajfault", "pgfree", "pgscan_kswapd",
                "pgscan_direct", "pgsteal", "pgpromote", "pgdemote");
        printf("%-12s  ", "");
        #endif

        if (!first_record) {
            memcpy(*deltas, *m, sizeof(__comp_t) * N_PAGING);
            (*m) += sizeof(__comp_t) * N_PAGING;
            
            ca[i].pgpgin = (unsigned long long) ((*deltas)[0] + pa[i].pgpgin);
            ca[i].pgpgout = (unsigned long long) ((*deltas)[1] + pa[i].pgpgout);
            ca[i].pgfault = (unsigned long long) ((*deltas)[2] + pa[i].pgfault);
            ca[i].pgmajfault = (unsigned long long) ((*deltas)[3] + pa[i].pgmajfault);
            ca[i].pgfree = (unsigned long long) ((*deltas)[4] + pa[i].pgfree);
            ca[i].pgscan_kswapd = (unsigned long long) ((*deltas)[5] + pa[i].pgscan_kswapd);
            ca[i].pgscan_direct = (unsigned long long) ((*deltas)[6] + pa[i].pgscan_direct);
            ca[i].pgsteal = (unsigned long long) ((*deltas)[7] + pa[i].pgsteal);
            ca[i].pgpromote = (unsigned long long) ((*deltas)[8] + pa[i].pgpromote);
            ca[i].pgdemote = (unsigned long long) ((*deltas)[9] + pa[i].pgdemote);
        }

        #ifdef VERBOSE
        printf("%9lu  %9lu  %9lu  %9lu  %9lu  %9lu  %9lu  %9lu  %9lu  %9lu\n",
                ca[i].pgpgin, ca[i].pgpgout, ca[i].pgfault, ca[i].pgmajfault, ca[i].pgfree,
                ca[i].pgscan_kswapd, ca[i].pgscan_direct, ca[i].pgsteal, ca[i].pgpromote, ca[i].pgdemote);
        #endif
    }
}

void read_io_stats(struct act_t **act, int curr, int prev, void **m, int first_record) {

    struct stats_io *ca = (struct stats_io *) (*act)->act[curr];
    struct stats_io *pa = (struct stats_io *) (*act)->act[prev];
    __comp_t **deltas = &((*act)->deltas);

    if (first_record) {
        *deltas = (__comp_t *)malloc(sizeof(__comp_t) * N_IO);
    }

    for (int i = 0; i < (*act)->nr; i++) {
        #ifdef VERBOSE
        printf("\n%-12s  %9s  %9s  %9s  %9s  %9s  %9s  %9s\n",
                "IO", "dk_drive", "dk_drive_rio", "dk_drive_wio", "dk_drive_dio", "dk_drive_rblk",
                "dk_drive_wblk", "dk_drive_dblk");
        printf("%-12s  ", "");

        #endif
        if (!first_record) {
            memcpy(*deltas, *m, sizeof(__comp_t) * N_IO);
            (*m) += sizeof(__comp_t) * N_IO;
    
            ca[i].dk_drive  = (unsigned long long) ((*deltas)[0] + pa[i].dk_drive);
            ca[i].dk_drive_rio  = (unsigned long long) ((*deltas)[1] + pa[i].dk_drive_rio);
            ca[i].dk_drive_wio  = (unsigned long long) ((*deltas)[2] + pa[i].dk_drive_wio);
            ca[i].dk_drive_dio  = (unsigned long long) ((*deltas)[3] + pa[i].dk_drive_dio);
            ca[i].dk_drive_rblk  = (unsigned long long) ((*deltas)[4] + pa[i].dk_drive_rblk);
            ca[i].dk_drive_wblk  = (unsigned long long) ((*deltas)[5] + pa[i].dk_drive_wblk);
            ca[i].dk_drive_dblk  = (unsigned long long) ((*deltas)[6] + pa[i].dk_drive_dblk);
        }

        #ifdef VERBOSE
        printf("%9llu  %9llu  %9llu  %9llu  %9llu  %9llu  %9llu\n",
                ca[i].dk_drive, ca[i].dk_drive_rio, ca[i].dk_drive_wio, ca[i].dk_drive_dio,
                ca[i].dk_drive_rblk, ca[i].dk_drive_wblk, ca[i].dk_drive_dblk);
        #endif
    }
}

void read_queue_stats(struct act_t **act, int curr, int prev, void **m, int first_record) {

    struct stats_queue *ca = (struct stats_queue *) (*act)->act[curr];
    struct stats_queue *pa = (struct stats_queue *) (*act)->act[prev];
    __comp_t **deltas = &((*act)->deltas);
    
    if (first_record) {
        *deltas = (__comp_t *)malloc(sizeof(__comp_t) * N_QUEUE);
    }

    for (int i = 0; i < (*act)->nr; i++) {
        #ifdef VERBOSE
        printf("\n%-12s  %12s  %12s  %12s  %12s  %12s  %12s\n",
                "QUEUE", "nr_running", "nr_threads", "load_avg_1", "load_avg_5", "load_avg_15", "procs_blocked");
        printf("%-12s  ", ""); 
        #endif

        if (!first_record) {
            memcpy(*deltas, *m, sizeof(__comp_t) * N_QUEUE);
            (*m) += sizeof(__comp_t) * N_QUEUE;
    
            ca[i].nr_running = (unsigned long long) ((*deltas)[0] + pa[i].nr_running);
            ca[i].nr_threads = (unsigned long long) ((*deltas)[1] + pa[i].nr_threads);
            ca[i].load_avg_1 = (unsigned long long) ((*deltas)[2] + pa[i].load_avg_1);
            ca[i].load_avg_5 = (unsigned long long) ((*deltas)[3] + pa[i].load_avg_5);
            ca[i].load_avg_15 = (unsigned long long) ((*deltas)[4] + pa[i].load_avg_15);
            ca[i].procs_blocked = (unsigned long long) ((*deltas)[5] + pa[i].procs_blocked);
        }

        #ifdef VERBOSE
        printf("%12llu  %12llu  %12u  %12u  %12u  %12llu\n",
                ca[i].nr_running, ca[i].nr_threads, ca[i].load_avg_1, ca[i].load_avg_5,
                ca[i].load_avg_15, ca[i].procs_blocked);
        #endif
    }
}

void decompress_record_hdr(struct record_header **curr_hdr, struct record_header *prev_hdr, void **m, unsigned int **deltas, int first_record) {
    struct record_header **curr = curr_hdr;
    struct record_header *prev = prev_hdr;

    #ifdef VERBOSE
    printf("\n%-12s  %9s  %9s  %9s  %9s  %9s  %9s  %9s\n",
            "RECORD HEADER", "uptime_cs", "ust_time", "extra_next", "record_type",
            "hour", "minute", "second");
    printf("%-12s  ", "");
    #endif

    if (first_record) {
        memcpy(*curr, *m, sizeof(struct record_header));
        (*m) += sizeof(struct record_header);
    }else {
        memcpy(*deltas, *m, sizeof(unsigned int) * N_RECORD_HDR_ULL);
        (*m) += sizeof(unsigned int) * N_RECORD_HDR_ULL;
        (*curr)->uptime_cs = (unsigned long long)((*deltas)[0] + prev->uptime_cs);
        (*curr)->ust_time = (unsigned long long)((*deltas)[1] + prev->ust_time);
        memcpy(&(*curr)->extra_next, *m, sizeof(unsigned int));
        (*m) += sizeof(unsigned int);
        memcpy(&(*curr)->record_type, *m, sizeof(unsigned char));
        (*m) +=sizeof(unsigned char);
        memcpy(&(*curr)->hour, *m, sizeof(unsigned char));
        (*m) += sizeof(unsigned char);
        memcpy(&(*curr)->minute, *m, sizeof(unsigned char));
        (*m) += sizeof(unsigned char);
        memcpy(&(*curr)->second, *m, sizeof(unsigned char));
        (*m) += sizeof(unsigned char);
    }    

    #ifdef VERBOSE
    printf("%9llu  %9llu  %9u  %9u  %9u  %9u  %9u\n",
        (*curr)->uptime_cs, (*curr)->ust_time, (*curr)->extra_next, (*curr)->record_type, (*curr)->hour, (*curr)->minute, (*curr)->second);
    #endif
}
