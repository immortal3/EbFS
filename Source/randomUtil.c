#include <stdlib.h>


// Function info : generating random char using inbuilt random number
char random_char()
{  
	char r = (char) (rand() % 255 + 1);
	return r;      
}

// Function info : generating random char array given length
// Important Note : don't forget to clear memory
char* generate_char_array(int size)
{
	char *random_char_array;
	random_char_array = (char *)malloc(sizeof(char) * size);
	for (int i = 0; i < size; ++i)
	{
		random_char_array[i] = random_char();
	}
	return (char *)random_char_array;
}
