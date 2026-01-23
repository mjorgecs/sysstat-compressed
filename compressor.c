// Manual calculation version - demonstrates how to calculate CPU percentages
// compile : gcc -Wall -Wextra -std=c99 -I ../sysstat-repo/ ../sysstat-repo/activity.c read_file_manual.c -o read_file_manual -lm

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

#include "utils.h"
#include "../sysstat-repo/sa.h"
#include "../sysstat-repo/rd_stats.h"
#include "../sysstat-repo/version.h"

extern struct activity *act[];
struct record_header *record_hdr[2];




int main(int argc, char **argv) {
    
    if(argc < 3) {
        usage(argv[0]);
    }
    
	char *path = argv[1];
    char *target = argv[2];

    int new_act = (argc - 3) ? (argc - 3) : 5;
    int *act_flags = malloc(new_act * sizeof(int));

    set_activity_flags(argc, new_act, argv, &act_flags);

    struct stat sbuf;
	int fd = open(path, O_RDONLY);
	fstat(fd, &sbuf);
	off_t len = sbuf.st_size;
    void *m_start = (void *) mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
    void *m = m_start;
    
    FILE * target_file = fopen(target, "w");

    // Write file_magic
    fwrite(m, FILE_MAGIC_SIZE, 1, target_file);
    m += FILE_MAGIC_SIZE;
    
    // Copy file header
    struct file_header hdr;
    memcpy((void *)&hdr, m, FILE_HEADER_SIZE);
    m += FILE_HEADER_SIZE;
    
    #ifdef VERBOSE
    printf("Linux %s (%s) \t%02u/%02u/%d \t_x86_64_\t(%d CPU)\n\n",
    hdr.sa_release, hdr.sa_nodename,
    hdr.sa_month, hdr.sa_day, hdr.sa_year + 1900,
    hdr.sa_cpu_nr > 1 ? hdr.sa_cpu_nr - 1 : 1);
    #endif
    
    unsigned int total_act = hdr.sa_act_nr;
    
    // Update number of activities in header and write it onto target file
    hdr.sa_act_nr = new_act;
    fwrite((void *)&hdr, FILE_HEADER_SIZE, 1, target_file);
    
    // Read and write file activity list
    int p, i, j;
    struct file_activity *fal = ((struct file_activity *)m);
    struct file_activity *file_act_lst[total_act]; 

    for (i = 0; i < (int)total_act; i++, fal++, m += FILE_ACTIVITY_SIZE) {
        file_act_lst[i] = fal;
        
        // select only activities to be processed
        if ((p = get_pos(act, fal->id)) < 0)
            continue;
        
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

        if (is_selected(fal->id, act_flags, new_act)) {
            fwrite(m, FILE_ACTIVITY_SIZE, 1, target_file);                 
        }
    }

    // Read records
    int curr = 1, prev = 0, first_record = 1;
    size_t data_size;
    __nr_t nr_value;
    
    // Process records, read until EOF
    do {

        record_hdr[curr] = ((struct record_header *) m);
        fwrite(m, RECORD_HEADER_SIZE, 1, target_file);
        m += RECORD_HEADER_SIZE;
        
        #ifdef VERBOSE
        printf("\n%02u:%02u:%02u --- COMPRESSED DATA WRITTEN (DELTAS): ", record_hdr[curr]->hour, record_hdr[curr]->minute, record_hdr[curr]->second);
        #endif

        // Read statistics for each activity
        for (i = 0; i < (int)new_act; i++) {
            fal = file_act_lst[i];
            
            if (fal->has_nr) {
                memcpy((void *)&nr_value, m, sizeof(__nr_t));
                m += sizeof(__nr_t);
            }
            else {
                nr_value = fal->nr;
            }

            if ((nr_value > 0) && ((p=get_pos(act, fal->id)) >= 0)) {
                data_size = (size_t) act[p]->fsize * (size_t) nr_value * (size_t) act[p]->nr2;
                
                if (is_selected(fal->id, act_flags, new_act)) {
                    if (fal->has_nr) fwrite((void *)&nr_value, sizeof(__nr_t), 1, target_file);
                    memcpy(act[p]->buf[curr], m, data_size);
                    act[p]->nr[curr] = nr_value;                    
                }

                m += data_size;

            } else {
                continue;
            }
            compress_stats((struct activity *)act[p], curr, prev, fal->id, target_file, first_record);
        }

        // Swap buffers
        int tmp = prev;
        prev = curr;
        curr = tmp;
        first_record = 0;

    } while(((size_t)(m - m_start) + RECORD_HEADER_SIZE) <= (size_t)len);

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

    free(act_flags);

    munmap(m_start, len);
    close(fd);
    fclose(target_file);
    
    return 0;
}
