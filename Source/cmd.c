#include <stdio.h>
#include "filesystem.c"

unsigned long hash(char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c;

    return hash;
}
int main(int argc, char const *argv[])
{
	char buffer[15];
	printf("\nRunning EbFs");
	int disk_status = disk_init("Disk_img_500",500);
	EbFs_format();
	char filename[28];
	char password[50];
	while(1)
	{
		// getting first command
		printf("\n>>>");
		scanf("%s",buffer);

		// Debug :: 
		printf("%ld\n",hash("createdir"));
		switch(hash(buffer))
		{
			//command : "ls"
			case 5863588:
				print_current_directory();
				break;

			//command : "readSB" read super block
			case 6953973951606:
				EbFs_read_superblock();
				break;

			// command : "readfile" read file
			case 7572877634027873:
				printf("Enter Filename(max size 28 char):\n");
				scanf("%s",filename);
				printf("Enter a password for file:\n");
				scanf("%s",password);
				EbFs_read_file(EbFs_file_inodenumber(filename),password);
				break;

			//command : "createfile"
			case 8246186021644673753:
				printf("Enter Filename(max size 28 char):\n");
				scanf("%s",filename);
				printf("Enter a password for file:\n");
				scanf("%s",password);
				char content[250000];
				printf("Enter a content of file\n");
				scanf("%s",content);
				EbFs_create_file(content, strlen(content),filename,false,password);
				break;

			// command : "createdir"
			case 249884424898321272:
				printf("Enter Filename(max size 28 char):\n");
				scanf("%s",filename);
				EbFs_create_file("",0,filename,true,"no need");
				break;
			// command : "exit"
			case 6385204799:
				return 0;
			default:
				printf("\n\nNo command Found\n\n");
				break;
		}
	
	}
	return 0;
}