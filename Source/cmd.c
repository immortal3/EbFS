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
	char filename[28];
	char password[50];
	int inodetmp;
	while(1)
	{
		// getting first command
		printf("\n\n>>>");
		scanf("%s",buffer);

		// Debug :: printf("hash : %ld\n",hash("format"));
		switch(hash(buffer))
		{	 
			//command : "ls"
			case 5863588:
				print_current_directory();
				break;

			// command : "format"
			case 6953516807342:
				EbFs_format();
				break;

			//command : "readSB" read super block
			case 6953973951606:
				EbFs_read_superblock();
				break;

			//command : "deletedir"
			case 249885286463998167:
				printf("Enter Directory Name:\n");
				scanf("%s",filename);
				//checking file is present or not
				inodetmp = EbFs_file_inodenumber(filename);
				if(inodetmp == -1)
				{
					printf("File doesn't exist\n");
				}
				else
				{
					EbFs_delete_directory(inodetmp);
				}
				
				break;

			//command : "cdparent"
			case 7572237649815350:
				if(go_back_to_parent_directory() > 0)
				{
					printf("Directory has been changed succesfully\n");
				}
				else
				{
					printf("Already in Root Directory\n");
				}
				
				break;

			// command : "readfile" read file
			case 7572877634027873:
				printf("Enter Filename(max size 28 char):\n");
				scanf("%s",filename);
				inodetmp = EbFs_file_inodenumber(filename);
				if(inodetmp == -1)
				{
					printf("File doesn't exist\n");
				}
				else
				{
					printf("Enter a password for file:\n");
					scanf("%s",password);
					EbFs_read_file(EbFs_file_inodenumber(filename),password);
				}
				break;

			//command : "createfile"
			case 8246186021644673753:
				printf("Enter Filename(max size 28 char):\n");
				scanf("%s",filename);
				printf("Enter a password for file:\n");
				scanf("%s",password);
				char content[250000];
				printf("Enter a content of file\n");
				scanf("%[^#]s",content);
				EbFs_create_file(content, strlen(content),filename,false,password);
				break;

			// command : "cd"
			case 5863276:
				printf("Enter Directory Name:\n");
				scanf("%s",filename);
				change_directory(filename);
				break;

			// command : "createdir"
			case 249884424898321272:
				printf("Enter Directory Name:\n");
				scanf("%s",filename);
				EbFs_create_file("",1,filename,true,"no need");
				break;

			// command : "deletefile"
			case 8246214453312011288:
				printf("Enter FileName:\n");
				scanf("%s",filename);
				//checking file is present or not
				inodetmp = EbFs_file_inodenumber(filename);
				if(inodetmp == -1)
				{
					printf("File doesn't exist\n");
				}
				else
				{
					EbFs_delete_file(inodetmp);
				}
				break;

			// command : "help"
			case 6385292014:
				printf("Help information\n");
				break;
			// command : "exit"
			case 6385204799:
				disk_close();
				return 0;
			default:
				printf("\n\nNo command Found\n\n");
				break;
		}
	
	}
	return 0;
}
