// Manual calculation version - demonstrates how to calculate CPU percentages
// compile : gcc -Wall -Wextra -std=c99 -I sysstat-repo/ read_file_manual.c -o read_file_manual -lm

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

#include "sa.h"
#include "rd_stats.h"

#include "pr_stats.h"
#include "version.h"

extern struct activity * act[];

int get_pos_simple(unsigned int act_id, struct file_activity *file_actlst[], int sa_act_nr)
{
	for (int i = 0; i < sa_act_nr; i++) {
		if (file_actlst[i]->id == act_id)
			return i;
	}
	return -1;
}

void print_cpu_stats(struct stats_cpu *scc, struct stats_cpu *scp, int nr_cpu, unsigned long long itv)
{
    (void)itv; // Unused - we calculate from jiffies directly
    
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
                      curr->cpu_softirq + curr->cpu_steal + 
                      curr->cpu_guest + curr->cpu_guest_nice;
        
        // Previous totals
        tot_jiffies_p = prev->cpu_user + prev->cpu_nice + prev->cpu_sys +
                        prev->cpu_idle + prev->cpu_iowait + prev->cpu_hardirq +
                        prev->cpu_softirq + prev->cpu_steal +
                        prev->cpu_guest + prev->cpu_guest_nice;
        
        // Calculate difference
        unsigned long long diff_total = tot_jiffies - tot_jiffies_p;
        
        if (diff_total == 0) {
            diff_total = 1;  // Avoid division by zero
        }
        
        // Calculate percentages
        double pc_user = (double)(curr->cpu_user - prev->cpu_user) * 100.0 / diff_total;
        double pc_nice = (double)(curr->cpu_nice - prev->cpu_nice) * 100.0 / diff_total;
        double pc_sys = (double)(curr->cpu_sys - prev->cpu_sys) * 100.0 / diff_total;
        double pc_iowait = (double)(curr->cpu_iowait - prev->cpu_iowait) * 100.0 / diff_total;
        double pc_steal = (double)(curr->cpu_steal - prev->cpu_steal) * 100.0 / diff_total;
        double pc_irq = (double)(curr->cpu_hardirq - prev->cpu_hardirq) * 100.0 / diff_total;
        double pc_soft = (double)(curr->cpu_softirq - prev->cpu_softirq) * 100.0 / diff_total;
        double pc_guest = (double)(curr->cpu_guest - prev->cpu_guest) * 100.0 / diff_total;
        double pc_gnice = (double)(curr->cpu_guest_nice - prev->cpu_guest_nice) * 100.0 / diff_total;
        double pc_idle = (double)(curr->cpu_idle - prev->cpu_idle) * 100.0 / diff_total;
        
        // Print CPU name (all for first, then individual CPUs)
        if (i == 0) {
            printf("%-12s", "all");
            printf("  %5.2f  %5.2f  %5.2f  %5.2f   %5.2f  %5.2f  %5.2f   %5.2f   %5.2f  %5.2f\n",
            pc_user, pc_nice, pc_sys, pc_iowait, pc_steal, 
            pc_irq, pc_soft, pc_guest, pc_gnice, pc_idle);

        } /*else {
            printf("%-12d", i - 1);
            printf("  %5.2f  %5.2f  %5.2f  %5.2f   %5.2f  %5.2f  %5.2f   %5.2f   %5.2f  %5.2f\n",
            pc_user, pc_nice, pc_sys, pc_iowait, pc_steal, 
            pc_irq, pc_soft, pc_guest, pc_gnice, pc_idle);
        }*/
        
    }
}

void print_memory_stats(struct stats_memory *smc, struct stats_memory *smp)
{
    (void)smp; // Unused for now
    
    printf("\n%-12s  %12s  %12s  %12s  %9s  %12s  %12s\n",
           "", "kbmemfree", "kbavail", "kbmemused", "%memused", 
           "kbbuffers", "kbcached");
    
    unsigned long long mem_total = smc->tlmkb;
    unsigned long long mem_free = smc->frmkb;
    unsigned long long mem_used = mem_total - mem_free;
    unsigned long long mem_avail = smc->availablekb;
    double pc_used = mem_total ? (double)mem_used * 100.0 / mem_total : 0.0;
    
    printf("%-12s  %12llu  %12llu  %12llu  %9.2f  %12llu  %12llu\n",
           "",
           mem_free, mem_avail, mem_used, pc_used,
           smc->bufkb, smc->camkb);
}

int main(int argc, char ** argv) 
{
    if(argc < 2) {
        fprintf(stderr, "Usage: %s <sa file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct stat sbuf;
	char * path = argv[1];
	int fd = open(path, O_RDONLY);
	fstat(fd, &sbuf);
	off_t len = sbuf.st_size;
    void * m_start = (void *) mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
    void * m = m_start;
    
    // Read file_magic
    m += FILE_MAGIC_SIZE;

    // Read file_header
    struct file_header *hdr = (struct file_header *)m;
    printf("Linux %s (%s) \t%02u/%02u/%d \t_x86_64_\t(%d CPU)\n\n",
           hdr->sa_release, hdr->sa_nodename,
           hdr->sa_month, hdr->sa_day, hdr->sa_year + 1900,
           hdr->sa_cpu_nr > 1 ? hdr->sa_cpu_nr - 1 : 1);
    m += FILE_HEADER_SIZE;
    
    // Read file_activity list
    struct file_activity *file_actlst[hdr->sa_act_nr]; 
    for (int i = 0; i < (int)hdr->sa_act_nr; i++) {
        file_actlst[i] = (struct file_activity *)m;
        m += FILE_ACTIVITY_SIZE;
    }

    // Find CPU and Memory activities
    int cpu_pos = get_pos_simple(A_CPU, file_actlst, hdr->sa_act_nr);
    int mem_pos = get_pos_simple(A_MEMORY, file_actlst, hdr->sa_act_nr);
    
    if (cpu_pos < 0) {
        fprintf(stderr, "CPU activity not found\n");
        exit(1);
    }

    // Allocate buffers for CPU stats
    int nr_cpu = file_actlst[cpu_pos]->nr;
    struct stats_cpu *cpu_curr = malloc(sizeof(struct stats_cpu) * nr_cpu);
    struct stats_cpu *cpu_prev = malloc(sizeof(struct stats_cpu) * nr_cpu);
    
    // Allocate buffers for memory stats
    struct stats_memory *mem_curr = malloc(sizeof(struct stats_memory));
    struct stats_memory *mem_prev = malloc(sizeof(struct stats_memory));


    // Read records
    int first_record = 1;
    int records_read = 0;
    
    while (1) {  // Read until EOF
        // Check if we have enough space for a record header
        if ((size_t)(m - m_start) + RECORD_HEADER_SIZE > (size_t)len) {
            break;  // Not enough data left for another record
        }
        
        struct record_header *rh = ((struct record_header *) m);
        m += RECORD_HEADER_SIZE;
        
        // Read statistics for each activity
        for (int i = 0; i < (int)hdr->sa_act_nr; i++) {
            struct file_activity *fal = file_actlst[i];
            
            __nr_t nr_value;
            if (fal->has_nr) {
                nr_value = *((__nr_t *) m);
                m += sizeof(__nr_t);
            }
            else {
                nr_value = fal->nr;
            }

            if (nr_value > 0) {
                size_t data_size = (size_t)fal->size * (size_t)nr_value * (size_t)fal->nr2;
                
                // Save CPU data (all CPUs at once)
                if (i == cpu_pos) {
                    memcpy(first_record ? cpu_prev : cpu_curr, m, data_size);
                }
                
                // Save Memory data (single structure)
                else if (i == mem_pos) {
                    memcpy(first_record ? mem_prev : mem_curr, m, data_size);
                }
                



                m += data_size;
            }
        }
        
        // After first record, we can print stats
        if (!first_record) {
            printf("\n%02u:%02u:%02u", rh->hour, rh->minute, rh->second);
            
            // Print CPU stats
            print_cpu_stats(cpu_curr, cpu_prev, nr_cpu, 100);
            
            // Print Memory stats if available
            if (mem_pos >= 0) {
                print_memory_stats(mem_curr, mem_prev);
            }
            
            // Swap buffers
            struct stats_cpu *tmp_cpu = cpu_prev;
            cpu_prev = cpu_curr;
            cpu_curr = tmp_cpu;
            
            struct stats_memory *tmp_mem = mem_prev;
            mem_prev = mem_curr;
            mem_curr = tmp_mem;
        }
        
        first_record = 0;
        records_read++;
    }

    printf("final size: %zu bytes\n", (size_t)(m - m_start));
    printf("total records read: %d\n", records_read);
    // Cleanup
    free(cpu_curr);
    free(cpu_prev);
    free(mem_curr);
    free(mem_prev);
    munmap(m_start, len);
    close(fd);
    
    printf("\n");
    return 0;
}
