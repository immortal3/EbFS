

//	
//	nblocks => total number of blocks
//	ninodeblocks => total number of inode blocks
//	ninodes => total number of inodes
struct superblock
{
	int nblocks;
	int ninodeblocks;
	int ninodes;
	int inodeofroot;
};