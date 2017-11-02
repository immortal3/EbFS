#include <mcrypt.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>


int encrypt(void* buffer,int buffer_len, char* key,int key_len )
{
	char* IV = "AAAAAAAAAAAAAAAB";
	  MCRYPT td = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);
	  int blocksize = mcrypt_enc_get_block_size(td);
	  if( buffer_len % blocksize != 0 ){return 1;}

	mcrypt_generic_init(td, key, key_len, IV);
	mcrypt_generic(td, buffer, buffer_len);
	mcrypt_generic_deinit (td);
	mcrypt_module_close(td);
	  
	return 0;
}

int decrypt(void* buffer,int buffer_len, char* key,int key_len )
{

	char* IV = "AAAAAAAAAAAAAAAB";
	MCRYPT td = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);
	int blocksize = mcrypt_enc_get_block_size(td);
	if( buffer_len % blocksize != 0 ){return 1;}
	  
	mcrypt_generic_init(td, key, key_len, IV);
	mdecrypt_generic(td, buffer, buffer_len);
	mcrypt_generic_deinit (td);
	mcrypt_module_close(td);
	  
	return 0;
}