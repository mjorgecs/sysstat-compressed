#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "utils.h"

struct record_header *record_hdr[2];

int main(int argc, char **argv) {

    if(argc < 3) {
        fprintf(stderr, "Usage: %s <compressed file> <output file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

	char *path = argv[1];
    char *target = argv[2];
    
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
    #endif

    // Read and write file activity lists
    int i;
    struct file_activity *fal = ((struct file_activity *)m);
    struct file_activity *file_actlst[hdr->sa_act_nr];
    struct act_t *comp_acts[hdr->sa_act_nr];

    for (i = 0; i < (int)hdr->sa_act_nr; i++, fal++, m += FILE_ACTIVITY_SIZE) {
        fwrite(m, FILE_ACTIVITY_SIZE, 1, target_file);
        file_actlst[i] = fal;
    }

    // Allocate record header buffers
    record_hdr[0] = malloc(RECORD_HEADER_SIZE);
    record_hdr[1] = malloc(RECORD_HEADER_SIZE);
    unsigned int *record_deltas = malloc(sizeof(unsigned int) * N_RECORD_HDR_ULL);
 
    int curr = 1, prev = 0, first_record = 1;
    size_t data_size;
    __nr_t nr_value;

    // Process compressed data, read until EOF
    do {

        decompress_record_hdr(&record_hdr[curr], record_hdr[prev], &m, &record_deltas, first_record);
        fwrite(record_hdr[curr], RECORD_HEADER_SIZE, 1, target_file);

        // Read statistics for each activity
        for (i = 0; i < (int)hdr->sa_act_nr; i++) {
            fal = file_actlst[i];
            
            if (fal->has_nr) {
                memcpy((void *)&nr_value, m, sizeof(__nr_t));
                fwrite(m, sizeof(__nr_t), 1, target_file);
                m += sizeof(__nr_t);
            }
            else {
                nr_value = fal->nr;
            }

            data_size = (size_t) fal->size * (size_t) nr_value * (size_t) fal->nr2;

            if (first_record) {
                // Allocate buffers for compressed activities
                comp_acts[i] = (struct act_t *) malloc(sizeof(struct act_t));
                comp_acts[i]->nr = nr_value;
                comp_acts[i]->act[0] = (void *)malloc(data_size);
                comp_acts[i]->act[1] = (void *)malloc(data_size);

                memcpy(comp_acts[i]->act[curr], m, data_size);
                m += data_size;
            }
            decompress_stats(&comp_acts[i], curr, prev, &m, first_record, fal->id);
            
            fwrite(comp_acts[i]->act[curr], data_size, 1, target_file);
        }

        // Swap buffers
        int tmp = prev;
        prev = curr;
        curr = tmp;
        first_record = 0;

    } while(((size_t)(m - m_start) + RECORD_HEADER_SIZE) <= (size_t)len);

    for (i = 0; i < (int)hdr->sa_act_nr; i++) {
        free(comp_acts[i]->act[0]);
        free(comp_acts[i]->act[1]);
        free(comp_acts[i]->deltas);
        free(comp_acts[i]);
    }

    free(record_hdr[0]);
    free(record_hdr[1]);
    free(record_deltas);

    munmap(m_start, len);
    close(fd);
    fclose(target_file);
    
    return 0;
}
