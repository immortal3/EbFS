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
// Total size of block_rw => 4096 bytes = 4kb
union block_rw
{
	char data[DISK_BLOCK_SIZE];
	// for reading suber block
	struct superblock sblock;
	// Total number of inode in given block
	struct inode iblks[MAX_INODE_IN_BLOCK];
	 // MAX_INODE_IN_BLOCK/32 which is 4kb / 32 = 128 files
	struct file_entry files[128]; 
};


void initRootDir()
{

	// initializing root directory
	EbFs_create_file("superblock",sizeof(char)*10,(char *)"root",true,"root");
}

// Function info : Displaying Current Directory's content
int print_current_directory()
{
	EbFs_read_file(CurrDirInode,"no key needed");
}




// Function info : using stack go back to parent directroy
int go_back_to_parent_directory()
{
	CurrDirInode = popdir();
}

// Function info : go into child directory like cd command in linux
int change_directory(const char  *dirname)
{

	int inodeblockno = CurrDirInode / 50 ;
	inodeblockno++;
	union block_rw inodeblock;
	disk_read(inodeblockno,inodeblock.data);

	if(inodeblock.iblks[CurrDirInode%50].mdata.filetype)
	{
		int i = 0;
		union block_rw readfile;
		disk_read(inodeblock.iblks[CurrDirInode%50].bdata.directblock[0],readfile.data);
		for (int i = 0; i < 128; ++i)
		{	
			if(!readfile.files[i].filename[0])
			{
				printf("No Directory Found !!\n");
				return -1;
			}
			if(!strcmp(readfile.files[i].filename,dirname))
			{
				pushdir(CurrDirInode);
				CurrDirInode = readfile.files[i].inodenumber;
				printf("CurrDirInode : %d\n",CurrDirInode );
				return 1;
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
	blk.sblock.ninodes = 50 * blk.sblock.ninodeblocks;
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
	memset(zero.data, 0, 4096);
    

    for(int pblock = 1; pblock <= blk.sblock.ninodeblocks; pblock++)
    {
    	//MAX_INODE_IN_BLOCK = 50 in one Physical Block
    	for (int i = 0; i <= MAX_INODE_IN_BLOCK ; ++i)
    	{
    		zero.iblks[i].addr =   i + (pblock-1)*MAX_INODE_IN_BLOCK;
    	}
        disk_write(pblock, zero.data);
    }

    memset(zero.data, 0, 4096);
   	for(int inode_block = blk.sblock.ninodeblocks + 1; inode_block <= blk.sblock.ninodeblocks + blk.sblock.nfreebitmapblocks ; inode_block++){
        disk_write(inode_block, zero.data);
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

	for (int i = 0; i < blk.sblock.nfreebitmapblocks; ++i)
	{
		disk_read(temp,bitmap.data);
		for (int j = 0; j < 4096; ++j)
		{
			if(bitmap.data[j]==0)
			{
				bitmap.data[j] = 1;
				disk_write(temp,bitmap.data);
				return blk.sblock.freebitmapstart + (i*4096) + j + 1;
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
	disk_read((CurrDirInode / 50) + 1,inodeblock.data);
	union block_rw readfile;
	disk_read(inodeblock.iblks[CurrDirInode%50].bdata.directblock[0],readfile.data);

	for (int i = 0; i < 128 ; ++i)
	{
		if(readfile.files[i].filename[0] == '\0')
		{
			// Debug :: printf("Entry File in current Directory\n");
			char tempcpy[28];
			strcpy(tempcpy,filename);
			memcpy(readfile.files[i].filename,tempcpy,sizeof(char) * 28);
			readfile.files[i].inodenumber = fileinode;
			disk_write(inodeblock.iblks[CurrDirInode%50].bdata.directblock[0],readfile.data);
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
	printf("File Inode : %d\n",newInodeAddr);
	int inodeblockno = newInodeAddr / 50 ;
	inodeblockno++;
	union block_rw inodeblock;
	disk_read(inodeblockno,inodeblock.data);

	//if less than 4kb(pblock size) * 12(direct blocks) = 48 kbytes
	if(size <= 4096 * 12)
	{	
		int c = 0;
		for (int i = size ; i > 0 ; i -= 4096 )
		{
			int newBlockAddr = EbFs_get_free_block();
			memset(blk.data,0,4096);
			inodeblock.iblks[newInodeAddr%50].mdata.filetype = isDir;
			inodeblock.iblks[newInodeAddr%50].bdata.directblock[c] = newBlockAddr;
			int slice_end = (c+1)*4096;
			if(i < 4096)
			{
				slice_end = (c)*4096 + i;
			}
			char *slice_ptr = slice_array(data,c*4096,slice_end);
			strncpy(blk.data, slice_ptr, 4096);
			if(!isDir)
			{
				encrypt(blk.data,4096, key,strlen(key));
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
	int inodeblockno = inodenumber / 50 ;
	inodeblockno++;
	union block_rw inodeblock;
	disk_read(inodeblockno,inodeblock.data);

	int i = 0;
	union block_rw readfile;

	while(true)
	{	
		if(i <= 12 && inodeblock.iblks[inodenumber%50].bdata.directblock[i] == 0)
		{
			i--;
			break;
		}
		i++;
	}
	int c = i;
	disk_read(inodeblock.iblks[inodenumber%50].bdata.directblock[i],readfile.data);
	decrypt(readfile.data,4096, key,strlen(key));

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

	if(size <= 4096 - z)
	{
		memcpy(readfile.data + z, data, size);
		printf("%s\n",readfile.data );
		encrypt(readfile.data,4096, key,strlen(key));
		disk_write(inodeblock.iblks[inodenumber%50].bdata.directblock[i],readfile.data);
	}
	else
	{
		memcpy(readfile.data + z, data, 4096 - z);
		disk_write(inodeblock.iblks[inodenumber%50].bdata.directblock[i],readfile.data);
		size = size - (4096 - z);
		union block_rw blk;
		for (int i = size ; i > 0 ; i -= 4096 )
		{
			int newBlockAddr = EbFs_get_free_block();
			memset(blk.data,0,4096);
			inodeblock.iblks[inodenumber%50].bdata.directblock[c] = newBlockAddr;
			// Debug line : printf("%d\n",inodeblock.iblks[newInodeAddr%50].bdata.directblock[c]);
			int slice_end = (c+1)*4096;
			if(i < 4096)
			{
				slice_end = (c)*4096 + i;
			}
			char *slice_ptr = slice_array(data,c*4096,slice_end);
			strncpy(blk.data, slice_ptr, 4096);
			// Debug line :: printf("writing data : %s\n",blk.data);
			disk_write(newBlockAddr,blk.data);
			c++;

		}
		disk_write(inodeblockno,inodeblock.data);
	}
}

// Function info : retunr inode number of given filename from current directory
int EbFs_file_inodenumber(char filename[])
{
	printing_util();
	int inodeblockno = CurrDirInode / 50 ;
	inodeblockno++;
	union block_rw inodeblock;
	disk_read(inodeblockno,inodeblock.data);

	if(inodeblock.iblks[CurrDirInode%50].mdata.filetype)
	{
		int i = 0;
		union block_rw readfile;
		disk_read(inodeblock.iblks[CurrDirInode%50].bdata.directblock[0],readfile.data);
		for (int i = 0; i < 128; ++i)
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

	int inodeblockno = inodenumber / 50 ;
	inodeblockno++;
	union block_rw inodeblock;
	disk_read(inodeblockno,inodeblock.data);
	if(inodeblock.iblks[inodenumber%50].mdata.filetype)
	{
		printf("Reading Directory\n");
		int i = 0;
		union block_rw readfile;
		disk_read(inodeblock.iblks[inodenumber%50].bdata.directblock[0],readfile.data);
		//decrypt(readfile.data,4096, key,strlen(key));
		for (int i = 0; i < 128; ++i)
		{	
			if(!readfile.files[i].filename[0])
			{
				break;
			}
			printf("\nFilename : %s Inode number : %d",readfile.files[i].filename,readfile.files[i].inodenumber);
		}
	}
	else 
	{
		int i = 0;
		union block_rw readfile;
		if(inodeblock.iblks[inodenumber%50].bdata.directblock[0] == 0)
		{
			printf("Empty file\n");
			return -1;
		}
		printf("\nReading file :");
		while(true)
		{	
			// reading 12 direct blocks
			if(i <= 12 && inodeblock.iblks[inodenumber%50].bdata.directblock[i] != 0)
			{

				disk_read(inodeblock.iblks[inodenumber%50].bdata.directblock[i],readfile.data);
				decrypt(readfile.data,4096, key,strlen(key));
				printf("%s\n",readfile.data);
			}
			else if(i <= 12 && inodeblock.iblks[inodenumber%50].bdata.directblock[i] == 0)
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
	int access_bitmap_block = blockno / 4096;
	int temp = blk.sblock.freebitmapstart + access_bitmap_block;
	union block_rw bitmap;
	printf("freeing block:%d\n",blockno);
	disk_read(temp,bitmap.data);
	bitmap.data[(blockno-blk.sblock.freebitmapstart-blk.sblock.nfreebitmapblocks+1)%4096] = 0;
	disk_write(temp,bitmap.data);
	return 1;

}

// Function info : deleting entry of file from current directory(call by delete file method)
int EbFs_delete_file_entery_in_dir(int fileinodenumber)
{
	int inodeblockno = CurrDirInode / 50 ;
	inodeblockno++;
	union block_rw inodeblock;
	disk_read(inodeblockno,inodeblock.data);

	if(inodeblock.iblks[CurrDirInode%50].mdata.filetype)
	{
		int i = 0;
		union block_rw readfile;
		disk_read(inodeblock.iblks[CurrDirInode%50].bdata.directblock[0],readfile.data);
		for (int i = 0; i < 128; ++i)
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
						disk_write(inodeblock.iblks[CurrDirInode%50].bdata.directblock[0],readfile.data);
						break;
					}
					printf("%s\n", readfile.files[j].filename);
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
	printing_util();
	int inodeblockno = inodenumber / 50 ;
	inodeblockno++;
	union block_rw inodeblock;
	disk_read(inodeblockno,inodeblock.data);
	if(inodeblock.iblks[inodenumber%50].mdata.filetype)
	{
		int i = 0;
		union block_rw readfile;
		disk_read(inodeblock.iblks[inodenumber%50].bdata.directblock[0],readfile.data);
		for (int i = 0; i < 128; ++i)
		{	
			if(!readfile.files[i].filename[0])
			{
				inodeblock.iblks[inodenumber%50].mdata.filetype = 0;
				disk_write(inodeblockno,inodeblock.data);
				EbFs_delete_file(inodenumber);
				break;
			}
			printf("%s\n",readfile.files[i].filename );
			EbFs_delete_file(readfile.files[i].inodenumber);
		}
	}
}


// Function info : deleting only file given filename
int EbFs_delete_file(int inodenumber)
{
	int inodeblockno = inodenumber / 50 ;
	inodeblockno++;
	union block_rw inodeblock;
	disk_read(inodeblockno,inodeblock.data);
	if(!inodeblock.iblks[inodenumber%50].mdata.filetype)
	{
		int i = 0;
		union block_rw readfile;
		while(true)
		{	
			// reading 12 direct blocks
			if(i <= 12 && inodeblock.iblks[inodenumber%50].bdata.directblock[i] != 0)
			{
				char random_4_kb[4096];
				strcpy(random_4_kb,generate_char_array(4096));
				EbFs_set_free_block(inodeblock.iblks[inodenumber%50].bdata.directblock[i]);
				disk_write(inodeblock.iblks[inodenumber%50].bdata.directblock[i],random_4_kb);
				inodeblock.iblks[inodenumber%50].bdata.directblock[i] = 0;
			}
			else if(i <= 12 && inodeblock.iblks[inodenumber%50].bdata.directblock[i] == 0)
			{
				break;
			}
			i++;
			
		}
		EbFs_delete_file_entery_in_dir(inodenumber);
		inodeblock.iblks[inodenumber%50].isallocated = 0;
		disk_write(inodeblockno,inodeblock.data);
		return 1;
	}
	else
	{
		printf("Delete Error:: Not Files\n");
		return -1;
	}
}
