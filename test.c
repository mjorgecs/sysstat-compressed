#include <stdio.h>
#include <stdlib.h>
 
#include "sa.h"
 
extern struct activity * act[];
int main() {
	printf("%s\n", act[0]->name);
	printf("%s\n", act[1]->name);
	printf("%s\n", act[2]->name);
	
/*
printf("has_nr=%d id=%u, nr_value=%d, fal_nr=%d, fal_nr2=%d, act_fsize=%d, act_msize=%d, act_nr_ini=%d, max_nr=%d\n",
fal->has_nr, fal->id, nr_value, fal->nr, fal->nr2, act[p]->fsize, act[p]->msize, act[p]->nr_ini, act[p]->nr_max);
*/
}