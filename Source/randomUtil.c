#include <time.h>
#include <stdlib.h>


// for safe random number generator
// cryptographically secure number
char crpyto_random_char()
{  
	char r = (char) (rand() % 255 + 1);
	return r;      
}


char* generate_char_array(int size)
{
	char *random_char_array;
	random_char_array = (char *)malloc(sizeof(char) * size);
	for (int i = 0; i < size; ++i)
	{
		random_char_array[i] = crpyto_random_char();
	}
	return (char *)random_char_array;
}
