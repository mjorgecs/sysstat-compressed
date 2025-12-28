#include <stdio.h>
#include <stdlib.h>
 
#include "sa.h"
 
extern struct activity * act[];
int main() {
	printf("%s\n", act[0]->name);
	printf("%s\n", act[1]->name);
	printf("%s\n", act[2]->name);
	
}