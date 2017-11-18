#include "disk.c"
#include "inode.c"
#include "superblock.c"
#include "util.c"
#include "randomUtil.c"
#include "crypto_util.c"
#include "stack.c"
#include "filesystem.h"

static int CurrDirInode = 0;

// Data structure for File entrying Directory
// Total size of file_entry => 32 bytes
struct file_entry
{
	char filename[28];
	int inodenumber;
};

// union block_rw => block read and write
// Total size of block_rw => DISK_BLOCK_SIZE bytes = 4kb
union block_rw
{
	char data[DISK_BLOCK_SIZE];
	// for reading suber block
	struct superblock sblock;
	// Total number of inode in given block
	struct inode iblks[MAX_INODE_IN_BLOCK];
	 // MAX_INODE_IN_BLOCK/32 which is 4kb / 32 = MAX_FILES_ENTRIES_IN_BLOCK = 128 files
	struct file_entry files[MAX_FILES_ENTRIES_IN_BLOCK]; 
};


void initRootDir()
{
	// initializing root directory
	EbFs_create_file("superblock",sizeof(char)*10,(char *)"root",true,"root");
}

// Function info : Displaying Current Directory's content
int print_current_directory()
{
	return EbFs_read_file(CurrDirInode,"no key needed");
}

// Function info : go back to root directory
void goback_to_root_directory()
{
	while(popdir() != 0);
	CurrDirInode = 0;
}

// Function info : using stack go back to parent directroy
int go_back_to_parent_directory()
{
	int temp = CurrDirInode;
	CurrDirInode = popdir();
	if(temp == CurrDirInode)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

// Function info : go into child directory like cd command in linux
int change_directory(const char  *dirname)
{

	int inodeblockno = CurrDirInode / MAX_INODE_IN_BLOCK ;
	inodeblockno++;
	union block_rw inodeblock;
	disk_read(inodeblockno,inodeblock.data);

	if(inodeblock.iblks[CurrDirInode%MAX_INODE_IN_BLOCK].mdata.filetype)
	{
		int i = 0;
		union block_rw readfile;
		disk_read(inodeblock.iblks[CurrDirInode%MAX_INODE_IN_BLOCK].bdata.directblock[0],readfile.data);
		for (int i = 0; i < MAX_FILES_ENTRIES_IN_BLOCK; ++i)
		{	
			if(!readfile.files[i].filename[0])
			{
				printf("No Directory Found !!\n");
				return -1;
			}
			if(!strcmp(readfile.files[i].filename,dirname))
			{
				if(debugFD(readfile.files[i].inodenumber,1) == 1)
				{
					pushdir(CurrDirInode);
					CurrDirInode = readfile.files[i].inodenumber;
					printf("CurrDirInode : %d\n",CurrDirInode );
					return 1;
				}
				else
				{
					printf("It is not directory\n");
					return -5;
				}
			}
		}
	}

}


// Function info : Formatting Disk and creating all four partitions
//				   creating superblock , inodeblocks , bitmap blocks and data blocks
int EbFs_format()
{

	printf("\nFormatting Disk\n");
	if(disk_size()  < MINIMUM_DISk_SIZE )
	{
		printf("Disk size is too small\n");
        return -1;
	}

	printf("Formatting disk of Size : %d bytes\n", disk_size() * 4 * 1024 );

	union block_rw blk;
	blk.sblock.nblocks = disk_size();
	// 10% of block can be used for storing inodes
	blk.sblock.ninodeblocks = disk_size()/10;
	struct inode temp;	
	// calculating total number of inode can be stored on file system
	blk.sblock.ninodes = MAX_INODE_IN_BLOCK * blk.sblock.ninodeblocks;
	blk.sblock.nfreebitmap = blk.sblock.nblocks - blk.sblock.ninodeblocks;
	blk.sblock.nfreebitmapblocks = blk.sblock.nfreebitmap / (4*1024);
	blk.sblock.freebitmapstart = blk.sblock.ninodeblocks + 1;
	blk.sblock.addrRootInode = 1;

	if(blk.sblock.nfreebitmapblocks == 0)
	{
		blk.sblock.nfreebitmapblocks = 1;
	}

	printf("Size of inode : %ld\n",sizeof(temp));
	// writing super block data
	disk_write(0,blk.data);

	// writing all inode block to zero
	union block_rw zero;
	memset(zero.data, 0, DISK_BLOCK_SIZE);
    

    for(int pblock = 1; pblock <= blk.sblock.ninodeblocks; pblock++)
    {
    	//MAX_INODE_IN_BLOCK = MAX_INODE_IN_BLOCK in one Physical Block
    	for (int i = 0; i <= MAX_INODE_IN_BLOCK ; ++i)
    	{
    		zero.iblks[i].addr =   i + (pblock-1)*MAX_INODE_IN_BLOCK;
    	}
        disk_write(pblock, zero.data);
    }

    memset(zero.data, 0, DISK_BLOCK_SIZE);
   	for(int inode_block = blk.sblock.ninodeblocks + 1; inode_block <= blk.sblock.ninodeblocks + blk.sblock.nfreebitmapblocks ; inode_block++){
        disk_write(inode_block, zero.data);
    }

    

    char random_4_kb[DISK_BLOCK_SIZE];

    for (int datablock = blk.sblock.ninodeblocks + blk.sblock.nfreebitmapblocks + 1; datablock < blk.sblock.nblocks; ++datablock)
    {
    	strcpy(random_4_kb,generate_char_array(DISK_BLOCK_SIZE));
    	disk_write(datablock, random_4_kb);
    }
    initRootDir();
    // future work : initializing disk with random numbers after inodes
    return 1;
}

// Function info : Reading all stored information in super blocks
int EbFs_read_superblock()
{
	union block_rw blk;
	disk_read(0,blk.data);
	printf("\n\nTotal Number of inodes : %d\n",blk.sblock.ninodes);
	printf("Total Number of inode blocks: %d\n",blk.sblock.ninodeblocks);
	printf("Total number of Bitmap:%d\n",blk.sblock.nfreebitmap );
	printf("Total Number of Free Bit map block: %d\n",blk.sblock.nfreebitmapblocks);
	printf("Free Bit map block start: %d\n",blk.sblock.freebitmapstart);
	printf("Address of Root inode number: %d\n",blk.sblock.addrRootInode);
	printf("Total number of Blocks :%d \n",blk.sblock.nblocks);
}

// Function info : Return free data block for storing inforamtion
int EbFs_get_free_block()
{
	union block_rw blk;
	disk_read(0,blk.data);
	int temp = blk.sblock.freebitmapstart;
	union block_rw bitmap;
	int overhead = (int) blk.sblock.nblocks / 2;

	while(overhead)
	{
		int rand_blk_int = rand() % (blk.sblock.nblocks + 1 - blk.sblock.freebitmapstart - blk.sblock.nfreebitmapblocks) + blk.sblock.freebitmapstart + blk.sblock.nfreebitmapblocks;
		int tempbitmap = (rand_blk_int / DISK_BLOCK_SIZE) + blk.sblock.freebitmapstart;
		disk_read(temp,bitmap.data);
		if(bitmap.data[rand_blk_int % DISK_BLOCK_SIZE]==0)
		{
				
				bitmap.data[rand_blk_int % DISK_BLOCK_SIZE] = 1;
				disk_write(temp,bitmap.data);
				return rand_blk_int;
		}
		overhead--;
	}
	for (int i = 0; i < blk.sblock.nfreebitmapblocks; ++i)
	{
		disk_read(temp,bitmap.data);
		for (int j = 0; j < DISK_BLOCK_SIZE; ++j)
		{
			if(bitmap.data[j]==0)
			{
				bitmap.data[j] = 1;
				disk_write(temp,bitmap.data);
				return blk.sblock.freebitmapstart + (i*DISK_BLOCK_SIZE) + j + 1;
			}
		}
		temp = temp + 1;
	}

}

// Function info : Registering file with current directory
int Ebfs_entery_file_in_dir(char filename[],int fileinode)
{	
	printf("Creating File with name :%s\n",filename );
	union block_rw inodeblock;
	disk_read((CurrDirInode / MAX_INODE_IN_BLOCK) + 1,inodeblock.data);
	union block_rw readfile;
	disk_read(inodeblock.iblks[CurrDirInode%MAX_INODE_IN_BLOCK].bdata.directblock[0],readfile.data);

	for (int i = 0; i < MAX_FILES_ENTRIES_IN_BLOCK ; ++i)
	{
		if(readfile.files[i].filename[0] == '\0')
		{
			
			char tempcpy[28];
			strcpy(tempcpy,filename);
			memcpy(readfile.files[i].filename,tempcpy,sizeof(char) * 28);
			readfile.files[i].inodenumber = fileinode;
			disk_write(inodeblock.iblks[CurrDirInode%MAX_INODE_IN_BLOCK].bdata.directblock[0],readfile.data);
			break;
		} 
	}
}


// Function info : get free inode for creating file or dir
int EbFs_get_free_inode()
{
	union block_rw sblk;
	union block_rw blk;
	disk_read(0,sblk.data);
	// reading from root data
	int temp = sblk.sblock.ninodeblocks;
	for (int i = 1; i < temp; ++i)
	{
		disk_read(i,blk.data);
		for (int j = 0; j < MAX_INODE_IN_BLOCK; ++j)
		{
			if(blk.iblks[j].isallocated == 0)
			{
				blk.iblks[j].isallocated = 1;
				disk_write(i,blk.data);
				return blk.iblks[j].addr;
			}
		}
	}
	return -1;

}

// Function info : creating file or dir with given content
int EbFs_create_file(char data[],long int size, char name[],bool isDir,char key[])
{
	union block_rw blk;
	int newInodeAddr = EbFs_get_free_inode();
	
	int inodeblockno = newInodeAddr / MAX_INODE_IN_BLOCK ;
	inodeblockno++;
	union block_rw inodeblock;
	disk_read(inodeblockno,inodeblock.data);

	//if less than 4kb(pblock size) * 12(direct blocks) = 48 kbytes
	if(size <= DISK_BLOCK_SIZE * 12)
	{	
		int c = 0;
		for (int i = size ; i > 0 ; i -= DISK_BLOCK_SIZE )
		{
			int newBlockAddr = EbFs_get_free_block();
			memset(blk.data,0,DISK_BLOCK_SIZE);
			inodeblock.iblks[newInodeAddr%MAX_INODE_IN_BLOCK].mdata.filetype = isDir;
			inodeblock.iblks[newInodeAddr%MAX_INODE_IN_BLOCK].bdata.directblock[c] = newBlockAddr;
			int slice_end = (c+1)*DISK_BLOCK_SIZE;
			if(i < DISK_BLOCK_SIZE)
			{
				slice_end = (c)*DISK_BLOCK_SIZE + i;
			}
			char *slice_ptr = slice_array(data,c*DISK_BLOCK_SIZE,slice_end);
			strncpy(blk.data, slice_ptr, DISK_BLOCK_SIZE);
			if(!isDir)
			{
				encrypt(blk.data,DISK_BLOCK_SIZE, key,strlen(key));
			}
			disk_write(newBlockAddr,blk.data);
			c++;

		}
	}
	disk_write(inodeblockno,inodeblock.data);

	//File Entry in current directory
	Ebfs_entery_file_in_dir(name,newInodeAddr);
			
	return 1;		
}

// Function info : appending existing file with new content
int EbFs_append_file(char data[], long int size, int inodenumber,char key[])
{	
	printf("calling append file\n");
	int inodeblockno = inodenumber / MAX_INODE_IN_BLOCK ;
	inodeblockno++;
	union block_rw inodeblock;
	disk_read(inodeblockno,inodeblock.data);

	int i = 0;
	union block_rw readfile;

	while(true)
	{	
		if(i <= 12 && inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].bdata.directblock[i] == 0)
		{
			i--;
			break;
		}
		i++;
	}
	int c = i;
	disk_read(inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].bdata.directblock[i],readfile.data);
	decrypt(readfile.data,DISK_BLOCK_SIZE, key,strlen(key));

	// getting length of content in last file
	int z = 0;
	while(true)
	{
		if(readfile.data[z] == '\0')
		{
			break;
		}
		z++;
	}

	if(size <= DISK_BLOCK_SIZE - z)
	{
		memcpy(readfile.data + z, data, size);
		printf("%s\n",readfile.data );
		encrypt(readfile.data,DISK_BLOCK_SIZE, key,strlen(key));
		disk_write(inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].bdata.directblock[i],readfile.data);
	}
	else
	{
		memcpy(readfile.data + z, data, DISK_BLOCK_SIZE - z);
		disk_write(inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].bdata.directblock[i],readfile.data);
		size = size - (DISK_BLOCK_SIZE - z);
		union block_rw blk;
		for (int i = size ; i > 0 ; i -= DISK_BLOCK_SIZE )
		{
			int newBlockAddr = EbFs_get_free_block();
			memset(blk.data,0,DISK_BLOCK_SIZE);
			inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].bdata.directblock[c] = newBlockAddr;
			int slice_end = (c+1)*DISK_BLOCK_SIZE;
			if(i < DISK_BLOCK_SIZE)
			{
				slice_end = (c)*DISK_BLOCK_SIZE + i;
			}
			char *slice_ptr = slice_array(data,c*DISK_BLOCK_SIZE,slice_end);
			strncpy(blk.data, slice_ptr, DISK_BLOCK_SIZE);
			disk_write(newBlockAddr,blk.data);
			c++;

		}
		disk_write(inodeblockno,inodeblock.data);
	}
}

// Function info : retunr inode number of given filename from current directory
int EbFs_file_inodenumber(char filename[])
{
	
	int inodeblockno = CurrDirInode / MAX_INODE_IN_BLOCK ;
	inodeblockno++;
	union block_rw inodeblock;
	disk_read(inodeblockno,inodeblock.data);

	if(inodeblock.iblks[CurrDirInode%MAX_INODE_IN_BLOCK].mdata.filetype)
	{
		int i = 0;
		union block_rw readfile;
		disk_read(inodeblock.iblks[CurrDirInode%MAX_INODE_IN_BLOCK].bdata.directblock[0],readfile.data);
		for (int i = 0; i < MAX_FILES_ENTRIES_IN_BLOCK; ++i)
		{	
			if(!readfile.files[i].filename[0])
			{
				break;
			}
			else if(!strcmp(readfile.files[i].filename,filename))
			{
				return readfile.files[i].inodenumber;
			}
		}
	}

	return -1;
}


// Function info : reading file or dir given inode number
int EbFs_read_file(int inodenumber,char key[])
{

	int inodeblockno = inodenumber / MAX_INODE_IN_BLOCK ;
	inodeblockno++;
	union block_rw inodeblock;
	disk_read(inodeblockno,inodeblock.data);
	if(inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].mdata.filetype)
	{
		printf("Reading Directory\n");
		int i = 0;
		union block_rw readfile;
		disk_read(inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].bdata.directblock[0],readfile.data);
		//decrypt(readfile.data,DISK_BLOCK_SIZE, key,strlen(key));
		for (int i = 0; i < MAX_FILES_ENTRIES_IN_BLOCK; ++i)
		{	
			if(!readfile.files[i].filename[0])
			{
				break;
			}
			printf("\nFilename : %s \tInode number : %d \tFileType: %d",readfile.files[i].filename,readfile.files[i].inodenumber,debugFD(readfile.files[i].inodenumber,1));
		}
	}
	else 
	{
		int i = 0;
		union block_rw readfile;
		if(inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].bdata.directblock[0] == 0)
		{
			printf("Empty file\n");
			return -1;
		}
		printf("\nReading file :");
		while(true)
		{	
			// reading 12 direct blocks
			if(i <= 12 && inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].bdata.directblock[i] != 0)
			{

				disk_read(inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].bdata.directblock[i],readfile.data);
				decrypt(readfile.data,DISK_BLOCK_SIZE, key,strlen(key));
				printf("%s\n",readfile.data);
			}
			else if(i <= 12 && inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].bdata.directblock[i] == 0)
			{
				break;
			}
			i++;
		}
	}

}

// Function info : releasing allocated data block for physically deleting file
int EbFs_set_free_block(int blockno)
{
	union block_rw blk;
	disk_read(0,blk.data);
	int access_bitmap_block = blockno / DISK_BLOCK_SIZE;
	int temp = blk.sblock.freebitmapstart + access_bitmap_block;
	union block_rw bitmap;
	printf("freeing block:%d\n",blockno);
	disk_read(temp,bitmap.data);
	bitmap.data[(blockno-blk.sblock.freebitmapstart-blk.sblock.nfreebitmapblocks+1)%DISK_BLOCK_SIZE] = 0;
	disk_write(temp,bitmap.data);
	return 1;

}

// Function info : deleting entry of file from current directory(call by delete file method)
int EbFs_delete_file_entery_in_dir(int fileinodenumber)
{
	int inodeblockno = CurrDirInode / MAX_INODE_IN_BLOCK ;
	inodeblockno++;
	union block_rw inodeblock;
	disk_read(inodeblockno,inodeblock.data);

	if(inodeblock.iblks[CurrDirInode%MAX_INODE_IN_BLOCK].mdata.filetype)
	{
		int i = 0;
		union block_rw readfile;
		disk_read(inodeblock.iblks[CurrDirInode%MAX_INODE_IN_BLOCK].bdata.directblock[0],readfile.data);
		for (int i = 0; i < MAX_FILES_ENTRIES_IN_BLOCK; ++i)
		{	
			if(!readfile.files[i].filename[0])
			{
				break;
			}
			else if(readfile.files[i].inodenumber == fileinodenumber)
			{
				for (int j = i; j < 4098; ++j)
				{
					if(!readfile.files[j].filename[0])
					{
						disk_write(inodeblock.iblks[CurrDirInode%MAX_INODE_IN_BLOCK].bdata.directblock[0],readfile.data);
						break;
					}
					strcpy(readfile.files[j].filename,readfile.files[j+1].filename);
					readfile.files[j].inodenumber = readfile.files[j+1].inodenumber;
				}
			}
		}
	}

	return -1;
}

// Function info : deleting directory by giving inodenumber from current directory
int EbFs_delete_directory(int inodenumber)
{
	int inodeblockno = inodenumber / MAX_INODE_IN_BLOCK ;
	inodeblockno++;
	union block_rw inodeblock;
	disk_read(inodeblockno,inodeblock.data);
	if(inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].mdata.filetype)
	{
		int i = 0;
		union block_rw readfile;
		disk_read(inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].bdata.directblock[0],readfile.data);
		for (int i = 0; i < MAX_FILES_ENTRIES_IN_BLOCK; ++i)
		{	
			if(!readfile.files[i].filename[0])
			{
				inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].mdata.filetype = 0;
				disk_write(inodeblockno,inodeblock.data);
				EbFs_delete_file(inodenumber);
				break;
			}
			printf("%s\n",readfile.files[i].filename );
			EbFs_delete_file(readfile.files[i].inodenumber);
		}
	}
}


// Function info : deleting only file given inodenumber
int EbFs_delete_file(int inodenumber)
{
	int inodeblockno = inodenumber / MAX_INODE_IN_BLOCK ;
	inodeblockno++;
	union block_rw inodeblock;
	disk_read(inodeblockno,inodeblock.data);
	if(!inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].mdata.filetype)
	{
		int i = 0;
		union block_rw readfile;
		while(true)
		{	
			// reading 12 direct blocks
			if(i <= 12 && inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].bdata.directblock[i] != 0)
			{
				char random_4_kb[DISK_BLOCK_SIZE];
				strcpy(random_4_kb,generate_char_array(DISK_BLOCK_SIZE));
				EbFs_set_free_block(inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].bdata.directblock[i]);
				disk_write(inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].bdata.directblock[i],random_4_kb);
				inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].bdata.directblock[i] = 0;
			}
			else if(i <= 12 && inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].bdata.directblock[i] == 0)
			{
				break;
			}
			i++;
			
		}
		EbFs_delete_file_entery_in_dir(inodenumber);
		inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].isallocated = 0;
		disk_write(inodeblockno,inodeblock.data);
		return 1;
	}
	else
	{
		printf("Delete Error:: Not Files\n");
		return -1;
	}
}


int debugFD(int inodenumber,int flag)
{
	// if flag is one return type of file like is file or directory
	if(flag == 1)
	{
		int inodeblockno = inodenumber / MAX_INODE_IN_BLOCK ;
		inodeblockno++;
		union block_rw inodeblock;
		disk_read(inodeblockno,inodeblock.data);
		return inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].mdata.filetype;
	}
	// only use in debugging
	// print information of allocated data blocks
	else if(flag == 2)
	{
		int inodeblockno = inodenumber / MAX_INODE_IN_BLOCK ;
		inodeblockno++;
		union block_rw inodeblock;
		disk_read(inodeblockno,inodeblock.data);
		int i = 0;
		while(true)
		{	
			// reading 12 direct blocks
			if(i <= 12 && inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].bdata.directblock[i] != 0)
			{
				printf("Data block allocated: %d %d\n",i,inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].bdata.directblock[i]);
			}
			else if(i <= 12 && inodeblock.iblks[inodenumber%MAX_INODE_IN_BLOCK].bdata.directblock[i] == 0)
			{
				return 1;
			}
			i++;
		}
	}
}
