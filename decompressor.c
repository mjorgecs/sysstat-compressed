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

struct record_header *record_hdr[2];

int main(int argc, char ** argv) {
    /*if(argc < 3) {
        fprintf(stderr, "Usage: %s <compressed file> <output file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }*/

	char * path = "target.bin";
    char * target = "file_decompressed.bin";
    
    struct stat sbuf;
	int fd = open(path, O_RDONLY);
	fstat(fd, &sbuf);
	off_t len = sbuf.st_size;
    void * m_start = (void *) mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
    void * m = m_start;
    
    FILE * target_file = fopen(target, "w");

    // Write file_magic
    fwrite(m, FILE_MAGIC_SIZE, 1, target_file);
    m += FILE_MAGIC_SIZE;

    // Write file_header
    struct file_header *hdr = (struct file_header *)m;
    #ifdef VERBOSE
    printf("Linux %s (%s) \t%02u/%02u/%d \t_x86_64_\t(%d CPU)\n\n",
           hdr->sa_release, hdr->sa_nodename,
           hdr->sa_month, hdr->sa_day, hdr->sa_year + 1900,
           hdr->sa_cpu_nr > 1 ? hdr->sa_cpu_nr - 1 : 1);
    printf("\nnr_act: %u", hdr->sa_act_nr);
    #endif
    fwrite(m, FILE_HEADER_SIZE, 1, target_file);
    m += FILE_HEADER_SIZE;
    
    // Read and write file_activity list
    int p, i, j, k;
    struct file_activity *fal = ((struct file_activity *)m);
    struct file_activity *file_actlst[hdr->sa_act_nr]; 


    int nr_cpu;

    for (i = 0; i < (int)hdr->sa_act_nr; i++, fal++, m += FILE_ACTIVITY_SIZE) {
        fwrite(m, FILE_ACTIVITY_SIZE, 1, target_file);
        file_actlst[i] = fal;

        // get the number of CPUs
        if (fal->id == A_CPU) {
            nr_cpu = fal->nr;
        }
        /*
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
        */
    }


    // Read records
    int curr = 1, prev = 0;
    int records_read = 0;
    int first_record = 1;
    
    struct stats_cpu **cpu[2];
    // alllocate mamory for CPUs
    cpu[0] = (struct stats_cpu **)malloc(sizeof(struct stats_cpu *) * nr_cpu);
    cpu[1] = (struct stats_cpu **)malloc(sizeof(struct stats_cpu *) * nr_cpu);

    for (i = 0; i < nr_cpu; i++) {
        cpu[0][i] = (struct stats_cpu *)malloc(sizeof(struct stats_cpu));
        cpu[1][i] = (struct stats_cpu *)malloc(sizeof(struct stats_cpu));
    }

    struct stats_memory *memory[2];
    struct stats_paging *paging[2];
    struct stats_io *io[2];
    struct stats_queue *queue[2];

    long cpu_deltas[N_CPU];
    long memory_deltas[N_MEMORY];
    long paging_deltas[N_PAGING];
    long io_deltas[N_IO];
    long queue_deltas[N_QUEUE];
    

    unsigned long long itv = 100;

    while (1) {  // Read until EOF
        // Check if we have enough space for a record header
        if ((size_t)(m - m_start) + RECORD_HEADER_SIZE > (size_t)len) {
            break;  // Not enough data left for another record
        }

        record_hdr[curr] = ((struct record_header *) m);
        fwrite(m, RECORD_HEADER_SIZE, 1, target_file);
        m += RECORD_HEADER_SIZE;
        
        #ifdef VERBOSE
        printf("\nTIME: %02u:%02u:%02u-------", record_hdr[curr]->hour, record_hdr[curr]->minute, record_hdr[curr]->second);
        #endif

        // Read statistics for each activity
        __nr_t nr_value;
        for (i = 0; i < (int)hdr->sa_act_nr; i++) {
            fal = file_actlst[i];
            
            if (fal->has_nr) {
                nr_value = *((__nr_t *) m);
                fwrite(m, sizeof(__nr_t), 1, target_file);
                m += sizeof(__nr_t);
            }
            else {
                nr_value = fal->nr;
            }
            /*
            p = get_pos(act, fal->id);
            if (nr_value > 0 && p >= 0) {
                size_t data_size = (size_t) act[p]->fsize * (size_t) nr_value * (size_t) act[p]->nr2;
                
                // Copy data
                memcpy(act[p]->buf[curr], m, data_size);
                act[p]->nr[curr] = nr_value;
                
                m += data_size;
            }
            */

            if (fal->id==A_CPU) {
                // Read CPU stats
                read_cpu_stats(&cpu[curr], &cpu[prev], nr_cpu, &m, first_record, cpu_deltas, target_file);
            }
            else if (fal->id==A_MEMORY) {
                // Read Memory stats
                read_memory_stats(&memory[curr], &memory[prev], target_file, first_record, &m, memory_deltas);
                fwrite((void *)memory[curr], sizeof(struct stats_memory), 1, target_file);
                printf("size_memory: %zu\n", sizeof(struct stats_memory));
            }
            else if (fal->id==A_PAGE) {
                // Read Paging stats
                read_paging_stats(&paging[curr], &paging[prev], itv, target_file, first_record, &m, paging_deltas);
                fwrite((void *)paging[curr], sizeof(struct stats_paging), 1, target_file);
                printf("size_paging: %zu\n", sizeof(struct stats_paging));                
            }
            else if (fal->id==A_IO) {
                // Read I/O stats
                read_io_stats(&io[curr], &io[prev], target_file, first_record, &m, io_deltas, itv);
                fwrite((void *)io[curr], sizeof(struct stats_io), 1, target_file);
                printf("size_io: %zu\n", sizeof(struct stats_io));
            }
            else if (fal->id==A_QUEUE) {
                // Read Queue stats
                read_queue_stats(&queue[curr], &queue[prev], target_file, first_record, &m, queue_deltas);
                fwrite((void *)queue[curr], sizeof(struct stats_queue), 1, target_file);
                printf("size_queue: %zu\n", sizeof(struct stats_queue));
            }
        }

        // Swap buffers
        int tmp = prev;
        prev = curr;
        curr = tmp;
        first_record = 0;
        records_read++;
    }

    // Cleanup
    for (i = 0; i < 2; i++) {
        //free(cpu[i]);
        free(memory[i]);
        free(paging[i]);
        free(io[i]);
        free(queue[i]);
    }

    munmap(m_start, len);
    close(fd);
    fclose(target_file);
    
    return 0;
}
