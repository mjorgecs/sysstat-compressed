// Manual calculation version - demonstrates how to calculate CPU percentages
// compile : gcc -Wall -Wextra -std=c99 -I ../sysstat-repo/ ../sysstat-repo/activity.c read_file_manual.c -o read_file_manual -lm

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

#include "pr_stats.h"
#include "../sysstat-repo/sa.h"
#include "../sysstat-repo/rd_stats.h"
#include "../sysstat-repo/version.h"

extern struct activity * act[];

struct record_header *record_hdr[2];


int get_pos(struct activity *act[], unsigned int act_flag) {
	int i;
	for (i = 0; i < NR_ACT; i++) {
		if (act[i]->id == act_flag)
			return i;
	}
	return -1;
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

        record_hdr[curr] = ((struct record_header *) m);
        m += RECORD_HEADER_SIZE;
        
        if (!first_record) {
            printf("\nTIME: %02u:%02u:%02u-------", record_hdr[curr]->hour, record_hdr[curr]->minute, record_hdr[curr]->second);
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

                unsigned long long itv = record_hdr[curr]->uptime_cs - record_hdr[prev]->uptime_cs;

                if (fal->id == A_CPU) {
                    // Print CPU stats
                    print_cpu_stats((struct stats_cpu *)act[p]->buf[curr], (struct stats_cpu *)act[p]->buf[prev], act[p]->nr_ini);
                }
                
                if (fal->id == A_MEMORY) {
                    // Print Memory stats
                    print_memory_stats((struct stats_memory *)act[p]->buf[curr]);
                }

                if (fal->id == A_PAGE) {
                    // Print Paging stats
                    print_paging_stats((struct stats_paging *)act[p]->buf[curr], (struct stats_paging *)act[p]->buf[prev], itv);
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
