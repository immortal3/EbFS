#include <stdio.h>
#include "disk.c"
#include "inode.c"
#include "superblock.c"
int main()
{

	struct inode temp;
	struct superblock s1;

	// creating virtual disk of 500 blocks 
	// total size = 500 * 4 kb
	int disk_status = disk_init("Disk_img_500",500);
	// Debug :: printf("Disk  number of blocks : %d\n",disk_size() );
	char data[DISK_BLOCK_SIZE] = "Hello";
	disk_write(25,data);
	char bufffer[DISK_BLOCK_SIZE];
	disk_read(25,bufffer);
	// Debug :: printf("Readed Value : %s\n", bufffer);

	printf("Size of inode : %ld byte(s)\n",sizeof(temp));
	printf("Size of Super block : %ld byte(s)\n",sizeof(s1) );

}

