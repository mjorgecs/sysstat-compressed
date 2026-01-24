#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "utils.h"


struct record_header *record_hdr[2];

int main(int argc, char **argv) {

    /*if(argc < 3) {
        fprintf(stderr, "Usage: %s <compressed file> <output file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }*/

	char *path = "target.bin";
    char *target = "file_decompressed.bin";
    
    struct stat sbuf;
	int fd = open(path, O_RDONLY);
	fstat(fd, &sbuf);
	off_t len = sbuf.st_size;
    void *m_start = (void *) mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
    void *m = m_start;
    
    FILE * target_file = fopen(target, "w");

    // Write file magic
    fwrite(m, FILE_MAGIC_SIZE, 1, target_file);
    m += FILE_MAGIC_SIZE;

    // Write file header
    struct file_header *hdr = (struct file_header *)m;
    fwrite(m, FILE_HEADER_SIZE, 1, target_file);
    m += FILE_HEADER_SIZE;

    #ifdef VERBOSE
    printf("Linux %s (%s) \t%02u/%02u/%d \t_x86_64_\t(%d CPU)\n\n",
           hdr->sa_release, hdr->sa_nodename,
           hdr->sa_month, hdr->sa_day, hdr->sa_year + 1900,
           hdr->sa_cpu_nr > 1 ? hdr->sa_cpu_nr - 1 : 1);
    printf("\nnr_act: %u", hdr->sa_act_nr);
    #endif

    // Read and write file activity lists
    int i;
    struct file_activity *fal = ((struct file_activity *)m);
    struct file_activity *file_actlst[hdr->sa_act_nr]; 

    int nr_cpu = 0;

    for (i = 0; i < (int)hdr->sa_act_nr; i++, fal++, m += FILE_ACTIVITY_SIZE) {
        fwrite(m, FILE_ACTIVITY_SIZE, 1, target_file);
        file_actlst[i] = fal;

        // Get the number of CPUs
        if (fal->id == A_CPU) {
            nr_cpu = fal->nr;
        }
    }

    // Allocate record header buffers
    record_hdr[0] = malloc(RECORD_HEADER_SIZE);
    record_hdr[1] = malloc(RECORD_HEADER_SIZE);

    int curr = 1, prev = 0, first_record = 1;
    
    struct stats_cpu **cpu[2];
    // alllocate mamory for CPUs
    cpu[0] = (struct stats_cpu **)malloc(sizeof(struct stats_cpu *) * nr_cpu);
    cpu[1] = (struct stats_cpu **)malloc(sizeof(struct stats_cpu *) * nr_cpu);
    unsigned int record_hdr_deltas[N_RECORD_HDR_ULL];

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

    // Process compressed data, read until EOF
    do {

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

    } while(((size_t)(m - m_start) + RECORD_HEADER_SIZE) <= (size_t)len);

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
