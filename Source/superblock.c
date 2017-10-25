
//
//	
//	nblocks => total number of blocks
//	ninodeblocks => total number of inode blocks
//	ninodes => total number of inodes
//  nfreebitmap => total number of free bitmap 
//	nfreebitmapblocks => total number of block reuqire to store bitmap
//	freebitmapstart => starting block of free bitmap
struct superblock
{
	int nblocks;
	int ninodeblocks;
	int ninodes;
	int addrRootInode;
	int nfreebitmap;
	int nfreebitmapblocks;
	int freebitmapstart;
};