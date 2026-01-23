#include "utils.h"

void usage(char * prog_name) {
    fprintf(stderr, "Usage: %s <sar file> <output file> <activities>\n", prog_name);
    fprintf(stderr, "Activities are:\n"
                      "  A_CPU       CPU usage statistics\n"
                      "  A_MEMORY    Memory usage statistics\n"
                      "  A_PAGE      Paging statistics\n"
                      "  A_IO        I/O statistics\n"
                      "  A_QUEUE     Queue statistics\n");
    fprintf(stderr, "If no activities are given, all default activities are processed.\n");
    exit(EXIT_FAILURE);
}


void set_activity_flags(int argc, int nr_act, char **argv, int **act_flags) {
	if (argc == 3) {
		// Set default activities
		(*act_flags)[0] = A_CPU;
		(*act_flags)[1] = A_MEMORY;
		(*act_flags)[2] = A_PAGE;
		(*act_flags)[3] = A_IO;
		(*act_flags)[4] = A_QUEUE;
	} else {
		// Set activities from command line
		for (int i = 0; i < nr_act; i++) {
			if (strcmp(argv[i + 3], "A_CPU") == 0) {
				(*act_flags)[i] = A_CPU;
			} else if (strcmp(argv[i + 3], "A_MEMORY") == 0) {
				(*act_flags)[i] = A_MEMORY;
			} else if (strcmp(argv[i + 3], "A_PAGE") == 0) {
				(*act_flags)[i] = A_PAGE;
			} else if (strcmp(argv[i + 3], "A_IO") == 0) {
				(*act_flags)[i] = A_IO;
			} else if (strcmp(argv[i + 3], "A_QUEUE") == 0) {
				(*act_flags)[i] = A_QUEUE;
			} else {
				fprintf(stderr, "Unknown activity: %s\n", argv[i + 3]);
				exit(EXIT_FAILURE);
			}
		}
	}
}


int is_selected(int act_id, int *act_flags, int nr_act) {
	for (int i = 0; i < nr_act; i++) {
		if (act_flags[i] == act_id) return 1;
	}
	return 0;
}

int get_pos(struct activity *act[], unsigned int act_flag) {
	for (int i = 0; i < NR_ACT; i++) {
		if (act[i]->id == act_flag) return i;
	}
	# ifdef DEBUG
		fprintf(stderr, "%s: Activity %u not found\n", __FUNCTION__, act_flag);
	# endif
	return -1;
}


void compress_stats(struct activity *act, int curr, int prev, unsigned int act_id, FILE *target_file, int first_record) {
	switch (act_id) {
		case A_CPU:
			write_cpu_stats(act->buf[curr], act->buf[prev], act->nr_ini, target_file, first_record);
			break;
		case A_MEMORY:
			write_memory_stats(act->buf[curr], act->buf[prev], target_file, first_record);
			break;
		case A_PAGE:
			write_paging_stats(act->buf[curr], act->buf[prev], target_file, first_record);
			break;
		case A_IO:
			write_io_stats(act->buf[curr], act->buf[prev], target_file, first_record);
			break;
		case A_QUEUE:
			write_queue_stats(act->buf[curr], act->buf[prev], target_file, first_record);
			break;
		default:
			break;
	}
}