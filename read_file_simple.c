// Simplified version to demonstrate printing stats like sar
// compile : gcc -Wall -Wextra -std=c99 -I sysstat-repo/ sysstat-repo/activity.c sa_common_stub.c read_file_simple.c -o read_file_simple -lm

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

#include "sa.h"
#include "pr_stats.h"
#include "rd_stats.h"
#include "version.h"

extern struct activity * act[];
unsigned int id_seq[NR_ACT];
uint64_t flags = 0x0000C000; // S_F_LOCAL_TIME | S_F_PREFD_TIME_OUTPUT
struct record_header record_hdr[3];

int get_pos(struct activity *act[], unsigned int act_flag)
{
	int i;
	for (i = 0; i < NR_ACT; i++) {
		if (act[i]->id == act_flag)
			return i;
	}
	return -1;
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
    struct file_magic *magic = (struct file_magic *)m;
    m += FILE_MAGIC_SIZE;

    // Read file_header
    struct file_header *hdr = (struct file_header *)m;
    printf("OS: %s %s\n", hdr->sa_sysname, hdr->sa_release);
    printf("Host: %s\n", hdr->sa_nodename);
    printf("CPUs: %d\n\n", hdr->sa_cpu_nr);
    m += FILE_HEADER_SIZE;
    
    // Read file_activity list
    int p, i, k;
    struct file_activity *fal = ((struct file_activity *)m);
    struct file_activity *file_actlst[hdr->sa_act_nr]; 

    int j=0;
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
            id_seq[j++] = fal->id;
        }
    }
	while (j < NR_ACT) {
		id_seq[j++] = 0;
	}

    // Read records
    int curr = 1, prev = 0;
    int first_record = 1;
    int records_read = 0;
    
    while (records_read < 5) {  // Read 5 records for demo
        struct record_header *rh = ((struct record_header *) m);
        memcpy(&record_hdr[curr], rh, RECORD_HEADER_SIZE);
        m += RECORD_HEADER_SIZE;
        
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
            else if (p >= 0) {
                act[p]->nr[curr] = 0;
            }
        }
        
        // After first record, we can print stats
        if (!first_record) {
            // Calculate interval
            unsigned long long itv;
            get_itv_value(&record_hdr[curr], &record_hdr[prev], &itv);
            
            // Print timestamp
            printf("%02u:%02u:%02u  ", rh->hour, rh->minute, rh->second);
            
            // Print each selected activity using its print function
            for (i = 0; i < NR_ACT; i++) {
                if (!id_seq[i])
                    continue;
                
                // Find activity position
                for (p = 0; p < NR_ACT; p++) {
                    if (act[p]->id == id_seq[i])
                        break;
                }
                
                if (p < NR_ACT && act[p]->nr[curr] > 0 && act[p]->f_print) {
                    // This calls the activity's print function which formats the output
                    (*act[p]->f_print)(act[p], prev, curr, itv);
                }
            }
            printf("\n");
        }
        
        first_record = 0;
        records_read++;
        
        // Swap buffers
        int tmp = prev;
        prev = curr;
        curr = tmp;
    }

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
