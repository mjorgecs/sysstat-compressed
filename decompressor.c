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
    if(argc < 3) {
        fprintf(stderr, "Usage: %s <compressed file> <output file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

	char * path = argv[1];
    char * target = argv[2];
    
    struct stat sbuf;
	int fd = open(path, O_RDONLY);
	fstat(fd, &sbuf);
	off_t len = sbuf.st_size;
    void * m_start = (void *) mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
    void * m = m_start;
    
    FILE * target_file = fopen(target, "w");

    // Read file_magic
    struct file_magic *magic = (struct file_magic *)m;
    m += FILE_MAGIC_SIZE;

    // Read file_header
    struct file_header *hdr = (struct file_header *)m;
    #ifdef VERBOSE
    printf("Linux %s (%s) \t%02u/%02u/%d \t_x86_64_\t(%d CPU)\n\n",
           hdr->sa_release, hdr->sa_nodename,
           hdr->sa_month, hdr->sa_day, hdr->sa_year + 1900,
           hdr->sa_cpu_nr > 1 ? hdr->sa_cpu_nr - 1 : 1);
    #endif
    m += FILE_HEADER_SIZE;
    
    // Read records
    int curr = 1, prev = 0, i = 0, j = 0;
    int records_read = 0;
    int first_record = 1;
    
    while (1) {  // Read until EOF
        // Check if we have enough space for a record header
        if ((size_t)(m - m_start) + RECORD_HEADER_SIZE > (size_t)len) {
            break;  // Not enough data left for another record
        }

        record_hdr[curr] = ((struct record_header *) m);
        m += RECORD_HEADER_SIZE;
        
        #ifdef VERBOSE
        printf("\nTIME: %02u:%02u:%02u-------", record_hdr[curr]->hour, record_hdr[curr]->minute, record_hdr[curr]->second);
        #endif

        // Read statistics for each activity
        for (i = 0; i < (int)hdr->sa_act_nr; i++) {
            unsigned int act_id = *((unsigned int *) m);
            m += sizeof(unsigned int);

            switch (act_id) {
            case A_CPU:
                // Read CPU stats
                /* code */
                break;
            
            case A_MEMORY:
                // Read Memory stats
                /* code */
                break;

            case A_PAGE:
                // Read Paging stats
                /* code */
                break;

            case A_IO:
                // Read I/O stats
                /* code */
                break;

            case A_QUEUE:
                // Read Queue stats
                /* code */
                break;
            
            default:
                break;
            }
        }

        // Swap buffers
        int tmp = prev;
        prev = curr;
        curr = tmp;

        first_record = 0;
        records_read++;
    }

    munmap(m_start, len);
    close(fd);
    fclose(target_file);
    
    return 0;
}
