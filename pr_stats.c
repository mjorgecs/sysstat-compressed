#include "pr_stats.h"


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

void print_memory_stats(struct stats_memory *smc) {
    
    /* Struct's variables */ 
    unsigned long long mem_frmkb = smc->frmkb;	/* MemFree */
	unsigned long long mem_bufkb = smc->bufkb;	/* Buffers */
	unsigned long long mem_camkb = smc->camkb;	/* Cached */
	unsigned long long mem_tlmkb = smc->tlmkb;	/* MemTotal */
	unsigned long long mem_frskb = smc->frskb;	/* SwapFree */
	unsigned long long mem_tlskb = smc->tlskb;	/* SwapTotal */
	unsigned long long mem_caskb = smc->caskb;	/* SwapCached */
	unsigned long long mem_comkb = smc->comkb;	/* Committed_AS */
	unsigned long long mem_activekb = smc->activekb;	/* Active */
	unsigned long long mem_inactkb = smc->inactkb;	/* Inactive */
	unsigned long long mem_dirtykb = smc->dirtykb;	/* Dirty */
	unsigned long long mem_anonpgkb = smc->anonpgkb;	/* AnonPages */
	unsigned long long mem_slabkb = smc->slabkb;	/* Slab */
	unsigned long long mem_kstackkb = smc->kstackkb;	/* KernelStack */
	unsigned long long mem_pgtblkb = smc->pgtblkb;	/* PageTables */
	unsigned long long mem_vmusedkb = smc->vmusedkb;	/* VmallocUsed */
	unsigned long long mem_availablekb = smc->availablekb;	/* MemAvailable */
	unsigned long long mem_shmemkb = smc->shmemkb;	/* Shmem */
        
    /* values for memused*/
    unsigned long long mem_used = mem_tlmkb - mem_availablekb;
    double pc_used = mem_tlmkb ? (double)mem_used * 100.0 / mem_tlmkb : 0.0;
    
    /* values for %commit*/
    double pc_commit = (mem_tlskb + mem_tlmkb) ? (double)mem_comkb * 100.0 / (mem_tlskb + mem_tlmkb) : 0.0;
    
    /* Print memory statistics */
    printf("\n%-12s  %12s  %12s  %12s  %9s  %12s  %12s  %12s  %9s  %12s  %12s  %12s  %12s\n",
           "MEMORY", 
           "kbmemfree", "kbavail", "kbmemused", "%memused", "kbbuffers", 
           "kbcached", "kbcommit", "%commit", "kbactive", "kbinact", "kbdirty", "kbshmem");

    printf("%-12s  %12llu  %12llu  %12llu  %9.2f  %12llu  %12llu  %12llu  %9.2f  %12llu  %12llu  %12llu  %12llu\n",
            "kB",
            mem_frmkb, mem_availablekb, mem_used, pc_used, mem_bufkb, 
            mem_camkb, mem_comkb, pc_commit, mem_activekb, mem_inactkb, mem_dirtykb, mem_shmemkb);
}

void print_paging_stats(struct stats_paging *spc, struct stats_paging *spp, unsigned long long itv) {
    struct stats_paging *curr = spc;
    struct stats_paging *prev = spp;

	if (!itv) {	/* Paranoia checking */
		itv = 1;
	}

    printf("\n%-12s  %5s  %5s  %5s  %5s  %5s  %5s  %5s  %5s  %5s  %5s\n",
            "PAGING", "pgpgin/s", "pgpgout/s", "fault/s", "majflt/s", "pgfree/s", "pgscank/s", "pgscand/s", "pgsteal/s", "pgprom/s", "pgdem/s");    
    
    printf("%-12s  %9.2f  %9.2f  %9.2f  %9.2f  %9.2f  %9.2f  %9.2f  %9.2f  %9.2f  %9.2f\n",
        "",
        S_VALUE(prev->pgpgin, curr->pgpgin, itv),
        S_VALUE(prev->pgpgout, curr->pgpgout, itv),
        S_VALUE(prev->pgfault, curr->pgfault, itv),
        S_VALUE(prev->pgmajfault, curr->pgmajfault, itv),
        S_VALUE(prev->pgfree, curr->pgfree, itv),
        S_VALUE(prev->pgscan_kswapd, curr->pgscan_kswapd, itv),
        S_VALUE(prev->pgscan_direct, curr->pgscan_direct, itv),
        S_VALUE(prev->pgsteal, curr->pgsteal, itv),
        S_VALUE(prev->pgpromote, curr->pgpromote, itv),
        S_VALUE(prev->pgdemote, curr->pgdemote, itv)
    );
}

