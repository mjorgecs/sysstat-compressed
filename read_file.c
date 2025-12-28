// compile : gcc -Wall -Wextra -std=c99 read_file.c -o read_file -lm

#include <stdio.h>
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

    size_t pos = 0;

    void * m = (void *) mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);

    
    // Read file_magic
    struct file_magic *magic = (struct file_magic *)m;
    printf("Sysstat version: %u\n", magic->sysstat_version);
    m += FILE_MAGIC_SIZE;
    pos += FILE_MAGIC_SIZE;

    // Read file_header
    struct file_header *hdr = (struct file_header *)m;
    printf("OS name: %s\n", hdr->sa_sysname);
    printf("Machine host: %s\n", hdr->sa_nodename);
    printf("Date: %u/%u/%d\n", hdr->sa_month, hdr->sa_day, hdr->sa_year);
    printf("OS release: %s\n", hdr->sa_release);
    printf("CPU count: %d\n", hdr->sa_cpu_nr);
    printf("sa_act_nr: %u\n", hdr->sa_act_nr);
    printf("extra next: %d\n", hdr->extra_next);
    m += FILE_HEADER_SIZE;
    pos += FILE_HEADER_SIZE;
    
    
    // Read file_activity
    int p, i, k, skip;
    struct file_activity *fal = ((struct file_activity *)m);
    struct file_activity *file_actlst[hdr->sa_act_nr]; 

    int j=0;
    for (i = 0; i < (int)hdr->sa_act_nr; i++, fal++, m += FILE_ACTIVITY_SIZE) {
        pos += FILE_ACTIVITY_SIZE;
        printf("\n\n");

        file_actlst[i] = fal;
        
        if ((p = get_pos(act, fal->id)) < 0)
			/* Unknown activity */
			continue;

        if ((fal->magic != act[p]->magic) && !DISPLAY_HDR_ONLY(flags)) {
			skip = TRUE;
		}
		else {
			skip = FALSE;
		}

        /* Atribute values*/
        printf("name: %s; id: %d; position: %d\n",act[p]->name, fal->id, p);
        for (k = 0; k < 3; k++) {
			act[p]->ftypes_nr[k] = fal->types_nr[k];
		}

		if (fal->size > act[p]->msize) {
			act[p]->msize = fal->size;
		}

		act[p]->nr_ini = fal->nr;
        printf("nr: %d\n", act[p]->nr_ini);
		act[p]->nr2    = fal->nr2;
        printf("nr2: %d\n", act[p]->nr2);
		act[p]->fsize  = fal->size;
        printf("fsize: %d\n", act[p]->fsize);

        /* Memory allocation*/
        act[p]->buf[0] = (void*)malloc((size_t) act[p]->msize * (size_t) act[p]->nr_ini * (size_t) act[p]->nr2);
        act[p]->nr_allocated = fal->nr;

        /*
		 * This is a known activity with a known format
		 * (magical number). Only such activities will be displayed.
		 * (Well, this may also be an unknown format if we have entered sadf -H.)
		 */
        if (!skip) {
            id_seq[j++] = fal->id;
        }
    }

	while (j < NR_ACT) {
		id_seq[j++] = 0;
	}


   //(size_t) act[p]->fsize * (size_t) nr_value * (size_t) act[p]->nr2

    




    // Read record_header
    long cnt = 0;
    int curr = 1;
    do{
        struct record_header *rh = ((struct record_header *) m);
        printf("time: %u:%u:%u\n", rh->hour, rh->minute, rh->second);
        // type : 1 - R_STATS
        //printf("type: %u\n", rh->record_type);
        // 0 - no extra stuctures
        //printf("extra: %u\n", rh->extra_next);
        m += RECORD_HEADER_SIZE;
        pos += RECORD_HEADER_SIZE;
        // Read statistics
        __nr_t nr_value;
        for (i = 0; i < (int)hdr->sa_act_nr; i++) {
            fal = file_actlst[i];

            if (file_actlst[i]->has_nr) {
                nr_value = *((__nr_t *) m);
                m += sizeof(__nr_t);
                pos += sizeof(__nr_t);
                //printf("has_nr, nr: %u\n", nr_value);
            }
            else {
                nr_value = file_actlst[i]->nr;
                //printf("no has_nr, nr: %d\n", nr_value);
            }

            p = get_pos(act, fal->id);
            if (nr_value > 0) {
                /*
                * Note: If msize was smaller than fsize,
                * then it has been set to fsize in check_file_actlst().
                */
               if(p == 0){
                    memcpy(act[p]->buf[0], m, (size_t) act[p]->fsize * (size_t) nr_value * (size_t) act[p]->nr2);
                    struct stats_cpu *test = (struct stats_cpu *) (act[p]->buf[0]);
                    printf("cpu_sys: %llu\n", test->cpu_sys);
               }
                //read(m, (char *) act[p]->buf[0], (size_t) act[p]->fsize * (size_t) nr_value * (size_t) act[p]->nr2);
                m += (size_t) act[p]->fsize * (size_t) nr_value * (size_t) act[p]->nr2;
                pos += (size_t) act[p]->fsize * (size_t) nr_value * (size_t) act[p]->nr2;
            }
        }


	}while (cnt++<15); // EOF

    printf("length: %ld\n", len);
    printf("pos: %ld\n", pos);



    for (i = 0; i < NR_ACT; i++) {

		if (act[i]->nr_allocated > 0) {
			for (j = 0; j < 3; j++) {
				if (act[i]->buf[j]) {
					free(act[i]->buf[j]);
					act[i]->buf[j] = NULL;
				}
			}
			act[i]->nr_allocated = 0;
		}
    }
    

    munmap(m, len);
    close(fd);

    return 0;
}