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
	// Debug :: 
	EbFs_read_superblock();
	// Debug :: 
	char filename[] = "morethan4kb.txt";
	// Important Note : tempstring should be greater than file size
	// Debug :: 
	char tempstring[13000] ;
	// Debug :: 
	// Debug :: 
	strcpy(tempstring,read_Whole_file(filename));
	// Debug :: 
	// Debug :: 
	char filename1[] = "test";
	EbFs_create_file(tempstring, sizeof(tempstring),filename1,false,"pass");
	 EbFs_read_file(1,"pass");
	char filename2[] = "testfolder";
	EbFs_create_file("",1,filename2,true,"123");

	// Debug :: EbFs_read_file(1);	

	// Debug ::  EbFs_delete_file(0);
	// Debug :: char tempdata[] = "helloworld";
	// Debug :: EbFs_append_file(tempdata , sizeof(tempdata), 1);
	char filename3[] = "testfolder";
	change_directory(filename3);
	char filename4[] = "newfile";
	char tempstring1[] = "new content";
	EbFs_create_file(tempstring1, sizeof(tempstring1),filename4,false,"12");
	print_current_directory();
	go_back_to_parent_directory();
	EbFs_delete_file(EbFs_file_inodenumber("test"));
	print_current_directory();

	// Debug :: printf(" test file inode : %d\n",EbFs_file_inodenumber("test") );
/*	printf("free block :%d\n",EbFs_get_free_block());
	EbFs_delete_directory(EbFs_file_inodenumber("testfolder"));
	printf("free block :%d\n",EbFs_get_free_block());
	print_current_directory();*/
	/*union block_rw temp_enc;
	strcpy(temp_enc.data,generate_char_array(4096));
	char *password = "key1223";
	char *plain_text = (char *)malloc(4096);
	memcpy(plain_text,temp_enc.data,4096);
	encrypt(temp_enc.data,4096, password,strlen(password));
	char *enc_msg = (char *)malloc(4096);
	memcpy(enc_msg,temp_enc.data,4096);
	decrypt(temp_enc.data,4096, password,strlen(password));
	
	if(!strcmp(plain_text,temp_enc.data))
	{
		printf("success\n");
	}*/
	disk_close();
}

