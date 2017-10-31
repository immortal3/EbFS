#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "aes.c"
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

int main(int argc, char const *argv[])
{

  char *msg;
  char *enc_msg;
  char *dec_msg;
  char key[]= "hello world";
  msg = generate_char_array(16);
  printf("%s\n",msg);
  enc_msg = (char *)encrypt((unsigned char *)msg, (unsigned char *)key, 16);
  dec_msg =  (char *)decrypt((unsigned char *)enc_msg, (unsigned char *)key, 16);
  printf("%s\n",dec_msg );
  if(!strcmp(dec_msg,msg))
  {
    printf("successful\n");
  }
  return 0;
}