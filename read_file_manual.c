// Manual calculation version - demonstrates how to calculate CPU percentages
// compile : gcc -Wall -Wextra -std=c99 -I ../sysstat-repo/ ../sysstat-repo/activity.c read_file_manual.c -o read_file_manual -lm

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

#include "../sysstat-repo/sa.h"
#include "../sysstat-repo/rd_stats.h"
#include "../sysstat-repo/version.h"

extern struct activity * act[];


int get_pos(struct activity *act[], unsigned int act_flag) {
	int i;
	for (i = 0; i < NR_ACT; i++) {
		if (act[i]->id == act_flag)
			return i;
	}
	return -1;
}

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

void print_memory_stats(struct stats_memory *smc, struct stats_memory *smp) {
    (void)smp; // Unused for now
    
    printf("\n%-12s  %12s  %12s  %12s  %9s  %12s  %12s\n",
           "MEMORY", "kbmemfree", "kbavail", "kbmemused", "%memused", 
           "kbbuffers", "kbcached");
    
    unsigned long long mem_total = smc->tlmkb;
    unsigned long long mem_free = smc->frmkb;
    unsigned long long mem_used = mem_total - mem_free;
    unsigned long long mem_avail = smc->availablekb;
    double pc_used = mem_total ? (double)mem_used * 100.0 / mem_total : 0.0;
    
    printf("%-12s  %12llu  %12llu  %12llu  %9.2f  %12llu  %12llu\n",
           "kb",
           mem_free, mem_avail, mem_used, pc_used,
           smc->bufkb, smc->camkb);
}

int main(int argc, char ** argv) {
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
    struct file_magic *magic = (struct file_magic *)m;
    m += FILE_MAGIC_SIZE;

    // Read file_header
    struct file_header *hdr = (struct file_header *)m;
    printf("Linux %s (%s) \t%02u/%02u/%d \t_x86_64_\t(%d CPU)\n\n",
           hdr->sa_release, hdr->sa_nodename,
           hdr->sa_month, hdr->sa_day, hdr->sa_year + 1900,
           hdr->sa_cpu_nr > 1 ? hdr->sa_cpu_nr - 1 : 1);
    m += FILE_HEADER_SIZE;
    
    // Read file_activity list
    int p, i, j, k;
    struct file_activity *fal = ((struct file_activity *)m);
    struct file_activity *file_actlst[hdr->sa_act_nr]; 

    for (i = 0; i < (int)hdr->sa_act_nr; i++, fal++, m += FILE_ACTIVITY_SIZE) {
        file_actlst[i] = fal;
        
        if ((p = get_pos(act, fal->id)) < 0)
			continue;

        // Set activity attributes
        for (k = 0; k < 3; k++) {
			act[p]->ftypes_nr[k] = fal->types_nr[k];
		}
		if (fal->size > act[p]->msize) {
			act[p]->msize = fal->size;
		}
		act[p]->nr_ini = fal->nr;
		act[p]->nr2    = fal->nr2;
		act[p]->fsize  = fal->size;

        // Allocate buffers for curr and prev
        act[p]->buf[0] = malloc((size_t) act[p]->msize * (size_t) act[p]->nr_ini * (size_t) act[p]->nr2);
        act[p]->buf[1] = malloc((size_t) act[p]->msize * (size_t) act[p]->nr_ini * (size_t) act[p]->nr2);
        act[p]->nr_allocated = fal->nr;


        if ((fal->magic == act[p]->magic)) {
            printf("Activity %s (id: %u) found. size: %d, nr: %u, nr2: %u\n",
                   act[p]->name, fal->id, fal->size, fal->nr, fal->nr2);
        }
    }


    // Read records
    int curr = 1, prev = 0;
    int first_record = 1;
    int records_read = 0;
    
    while (1) {  // Read until EOF
        // Check if we have enough space for a record header
        if ((size_t)(m - m_start) + RECORD_HEADER_SIZE > (size_t)len) {
            break;  // Not enough data left for another record
        }

        struct record_header *rh = ((struct record_header *) m);
        m += RECORD_HEADER_SIZE;
        
        if (!first_record) {
            printf("\nTIME: %02u:%02u:%02u-------", rh->hour, rh->minute, rh->second);
        }

        // Read statistics for each activity
        __nr_t nr_value;
        for (i = 0; i < (int)hdr->sa_act_nr; i++) {
            fal = file_actlst[i];
            
            if (fal->has_nr) {
                nr_value = *((__nr_t *) m);
                m += sizeof(__nr_t);
            }
            else {
                nr_value = fal->nr;
            }

            p = get_pos(act, fal->id);
            if (nr_value > 0 && p >= 0) {
                size_t data_size = (size_t) act[p]->fsize * (size_t) nr_value * (size_t) act[p]->nr2;
                
                // Copy data
                memcpy(act[p]->buf[curr], m, data_size);
                act[p]->nr[curr] = nr_value;
                
                m += data_size;
            }

            if (!first_record) {

                if (fal->id == A_CPU) {
                    // Print CPU stats
                    print_cpu_stats((struct stats_cpu *)act[p]->buf[curr], (struct stats_cpu *)act[p]->buf[prev], act[p]->nr_ini);
                }
                
                if (fal->id == A_MEMORY) {
                    // Print Memory stats
                    print_memory_stats((struct stats_memory *)act[p]->buf[curr], (struct stats_memory *)act[p]->buf[prev]);
                }
            }            
        }

        // Swap buffers
        int tmp = prev;
        prev = curr;
        curr = tmp;

        first_record = 0;
        records_read++;
    }

    //printf("final size: %zu bytes\n", (size_t)(m - m_start));
    //printf("total records read: %d\n", records_read);


    // Cleanup
    for (i = 0; i < NR_ACT; i++) {
		if (act[i]->nr_allocated > 0) {
			for (j = 0; j < 2; j++) {
				if (act[i]->buf[j]) {
					free(act[i]->buf[j]);
					act[i]->buf[j] = NULL;
				}
			}
			act[i]->nr_allocated = 0;
		}
    }

    munmap(m_start, len);
    close(fd);
    
    return 0;
}
