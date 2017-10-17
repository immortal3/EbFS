#include <stdio.h>
#include "filesystem.c"
int main()
{

	struct inode temp;

	// creating virtual disk of 500 blocks 
	// total size = 500 * 4 kb
	int disk_status = disk_init("Disk_img_500",500);
	// Debug :: printf("Disk  number of blocks : %d\n",disk_size() );
	// Debug :: char data[DISK_BLOCK_SIZE] = "Hello";
	// Debug :: disk_write(25,data);
 	// Debug :: char bufffer[DISK_BLOCK_SIZE];
	// Debug :: disk_read(25,bufffer);
	// Debug :: printf("Readed Value : %s\n", bufffer);

	// Debug :: printf("Size of inode : %ld byte(s)\n",sizeof(temp));
	// Debug :: printf("Size of Super block : %ld byte(s)\n",sizeof(s1) );
	// Debug :: EbFs_format();
	// Debug :: EbFs_read_superblock();
	char tempstring[] = "hello world";
	
	EbFs_create_file(tempstring,sizeof(tempstring));
	disk_close();

}

