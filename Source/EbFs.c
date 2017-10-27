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
	// Debug :: 
	EbFs_format();
	// Debug :: EbFs_read_superblock();
	// Debug :: 
	char filename[] = "morethan4kb.txt";
	// Important Note : tempstring should be greater than file size
	// Debug :: 
	char tempstring[13000] ;
	// Debug :: 
	char *wholefile = read_Whole_file(filename);
	// Debug :: 
	strcpy(tempstring,read_Whole_file(filename));
	// Debug :: 
	free(wholefile);
	// Debug :: 
	char filename1[] = "test";
	EbFs_create_file(tempstring, sizeof(tempstring),filename1,false);
	char filename2[] = "testfolder";
	EbFs_create_file("",1,filename2,true);

	// Debug :: EbFs_read_file(1);	

	// Debug ::  EbFs_delete_file(0);
	// Debug :: char tempdata[] = "helloworld";
	// Debug :: EbFs_append_file(tempdata , sizeof(tempdata), 1);
	char filename3[] = "testfolder";
	change_directory(filename3);
	char filename4[] = "newfile";
	char tempstring1[] = "new content";
	EbFs_create_file(tempstring1, sizeof(tempstring1),filename4,false);
	print_current_directory();
	EbFs_read_file(0);
	disk_close();
}

