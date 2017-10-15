#include "disk.c"
#include "inode.c"
#include "superblock.c"
#include "filesystem.h"

union block_write
{
	char data[DISK_BLOCK_SIZE];
	struct superblock sblock;
};


int EbFs_format()
{
	if(disk_size()  < MINIMUM_DISk_SIZE )
	{
		printf("Disk size is too small\n");
        return -1;
	}

	printf("Formatting disk of Size : %d bytes\n", disk_size() * 4 * 1024 );

	union block_write blk;
	blk.sblock.nblocks = disk_size();
	// 10% of block can be used for storing inodes
	blk.sblock.ninodeblocks = disk_size()/10;
	struct inode temp;	
	// calculating total number of inode can be stored on file system
	blk.sblock.ninodes = blk.sblock.ninodeblocks * 4 * 1024 / sizeof(temp);
	printf("Total number of inodes can be stored is : %d \n", blk.sblock.ninodes);

	// writing super block data
	disk_write(0,blk.data);

	// writing all inode block to zero
	union block_write zero;
	memset(zero.data, 0, 4096);
    

    for(int inode_block = 1; inode_block <= blk.sblock.ninodeblocks; inode_block++){
        disk_write(inode_block, zero.data);
    }
    
    // future work : initializing disk with random numbers after inodes
    return 1;
}


int EbFs_create_file()
{

}

int EbFs_create_dir()
{
	
}

int EbFs_delete_file()
{
	
}