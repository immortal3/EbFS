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
};


int EbFs_format()
{

	formating_operation();
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
	blk.sblock.ninodeblocks = disk_size()/10 + 1;
	struct inode temp;	
	// calculating total number of inode can be stored on file system
	blk.sblock.ninodes = (blk.sblock.ninodeblocks * 4 * 1024) / sizeof(temp);
	blk.sblock.nfreebitmap = blk.sblock.nblocks - blk.sblock.ninodeblocks;
	blk.sblock.nfreebitmapblocks = blk.sblock.nfreebitmap / (4*1024*8);
	blk.sblock.freebitmapstart = blk.sblock.ninodeblocks + 1;
	if(blk.sblock.nfreebitmapblocks == 0)
	{
		blk.sblock.nfreebitmapblocks = 1;
	}
	printf("Total number of inodes can be stored is : %d \n", blk.sblock.ninodes);
	// writing super block data
	disk_write(0,blk.data);

	// writing all inode block to zero
	union block_rw zero;
	memset(zero.data, 0, 4096);
    

    for(int inode_block = 1; inode_block <= blk.sblock.ninodeblocks; inode_block++)
    {
        disk_write(inode_block, zero.data);
    }

   	for(int inode_block = blk.sblock.ninodeblocks + 1; inode_block <= blk.sblock.ninodeblocks + blk.sblock.nfreebitmapblocks ; inode_block++){
        disk_write(inode_block, zero.data);
    }
    
    // future work : initializing disk with random numbers after inodes
    return 1;
}

int EbFs_read_superblock()
{
	formating_operation();
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
	disk_read(temp,bitmap.data);
	for (int i = 0; i < 4096; ++i)
	{
		if(bitmap.data[i]==0)
		{
			bitmap.data[i]=1;
			disk_write(temp,bitmap.data);
			return blk.sblock.freebitmapstart + i + 1;
		}
	}

}

int EbFs_create_file(char data[],long int size)
{
	formating_operation();
	printf("Free block found at %d",EbFs_get_free_block());
	
}

int EbFs_create_dir()
{
	
}

int EbFs_delete_file()
{
	
}