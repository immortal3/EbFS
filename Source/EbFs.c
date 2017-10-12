#include <stdio.h>
#include "disk.c"


int main()
{
	// creating virtual disk of 500 blocks 
	// total size = 500 * 4 kb
	int disk_status = disk_init("Disk_img_500",500);
	printf("Disk  number of blocks : %d\n",disk_size() );
	char data[DISK_BLOCK_SIZE] = "Hello";
	disk_write(25,data);
	char bufffer[DISK_BLOCK_SIZE];
	disk_read(25,bufffer);
	printf("Readed Value : %s\n", bufffer);
}

