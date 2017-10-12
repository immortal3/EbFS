#include <time.h>
#include <stdlib.h>


// for safe random number generator
// cryptographically secure number
int crpyto_random_number():
	srand(time(NULL));   
	int r = rand();
	return r;      