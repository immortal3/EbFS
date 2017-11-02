#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * MCrypt API available online:
 * http://linux.die.net/man/3/mcrypt
 */
#include <mcrypt.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>



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

int encrypt(
    void* buffer,
    int buffer_len, /* Because the plaintext could include null bytes*/
    char* IV, 
    char* key,
    int key_len 
){
  MCRYPT td = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);
  int blocksize = mcrypt_enc_get_block_size(td);
  if( buffer_len % blocksize != 0 ){return 1;}

  mcrypt_generic_init(td, key, key_len, IV);
  mcrypt_generic(td, buffer, buffer_len);
  mcrypt_generic_deinit (td);
  mcrypt_module_close(td);
  
  return 0;
}

int decrypt(
    void* buffer,
    int buffer_len,
    char* IV, 
    char* key,
    int key_len 
){
  MCRYPT td = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);
  int blocksize = mcrypt_enc_get_block_size(td);
  if( buffer_len % blocksize != 0 ){return 1;}
  
  mcrypt_generic_init(td, key, key_len, IV);
  mdecrypt_generic(td, buffer, buffer_len);
  mcrypt_generic_deinit (td);
  mcrypt_module_close(td);
  
  return 0;
}

void display(char* ciphertext, int len){
  int v;
  for (v=0; v<len; v++){
    printf("%d ", ciphertext[v]);
  }
  printf("\n");
}

int main()
{
  MCRYPT td, td2;
  char * plaintext = generate_char_array(4096);
  char* IV = "AAAAAAAAAAAAAAAB";
  char *key = "0123456789abcdef";
  int keysize = 16; /* 128 bits */
  char* buffer;
  int buffer_len = 4096;

  buffer = (char *)calloc(1, buffer_len);
  strncpy(buffer, plaintext, buffer_len);

  printf("==C==\n");
  //printf("plain:   %s\n", plaintext);
  encrypt(buffer, buffer_len, IV, key, keysize); 
  //printf("cipher:  "); display(buffer , buffer_len);
  char* IVI = "AAAAAAAAAAAAAAAA";
  decrypt(buffer, buffer_len, IV, key, keysize);
  //printf("decrypt: %s\n", buffer);
  if(!strcmp(plaintext,buffer))
  {
    printf("succesfull\n");
  }
  return 0;
}