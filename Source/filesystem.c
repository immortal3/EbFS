#include "disk.c"
#include "inode.c"
#include "superblock.c"
#include "filesystem.h"
#include "util.c"

union block_rw
{
	char data[DISK_BLOCK_SIZE];
	struct superblock sblock;
	struct inode iblock;
	struct inode iblks[MAX_INODE_IN_BLOCK];
};


int EbFs_format()
{

	printing_util();
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
    
    // future work : initializing disk with random numbers after inodes
    return 1;
}

int EbFs_read_superblock()
{
	printing_util();
	union block_rw blk;
	disk_read(0,blk.data);
	printf("\n\nTotal Number of inodes : %d\n",blk.sblock.ninodes);
	printf("Total Number of inode blocks: %d\n",blk.sblock.ninodeblocks);
	printf("Total Number of Free Bit map block: %d\n",blk.sblock.nfreebitmapblocks);
}


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
				printf(" %d is free inode\n",blk.iblks[j].addr);
				return 0;
			}
		}
	}
	return -1;

}

int EbFs_create_file(char data[],long int size)
{
	printing_util();

	union block_rw blk;
	int newInodeAddr = EbFs_get_free_inode();
	
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
			inodeblock.iblks[newInodeAddr%50].bdata.directblock[c] = newBlockAddr;
			// Debug line : printf("%d\n",inodeblock.iblks[newInodeAddr%50].bdata.directblock[0]);
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
	}	
	disk_write(inodeblockno,inodeblock.data);
			
	return 1;	
	
}

int EbFs_read_file(int inodenumber)
{
	int inodeblockno = inodenumber / 50 ;
	inodeblockno++;
	union block_rw inodeblock;
	disk_read(inodeblockno,inodeblock.data);

	// For checking allocated physical block
	/*
	for (int i = 0; i < 12; ++i)
	{
	 	Debug line : printf("Allocated BLock : %d \n", inodeblock.iblks[inodenumber%50].bdata.directblock[i]);
	} 
	*/
	int i = 0;
	union block_rw readfile;
	printf("\nReading file :");
	while(true)
	{	
		// reading 12 direct blocks
		if(i <= 12 && inodeblock.iblks[inodenumber%50].bdata.directblock[i] != 0)
		{
			disk_read(inodeblock.iblks[inodenumber%50].bdata.directblock[i],readfile.data);
			printf("%s\n",readfile.data);
		}
		else if(i <= 12 && inodeblock.iblks[inodenumber%50].bdata.directblock[i] == 0)
		{
			break;
		}
		i++;
	}

}

int EbFs_create_dir(char* name)
{

}

int EbFs_delete_file()
{
	
}