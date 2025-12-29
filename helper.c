#include "utils.h"

int get_pos(struct activity *act[], unsigned int act_flag) {
	int i;
	for (i = 0; i < NR_ACT; i++) {
		if (act[i]->id == act_flag)
			return i;
	}
	return -1;
}