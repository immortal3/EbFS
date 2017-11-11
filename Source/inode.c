#include "inode.h"
#include <stdbool.h>

// Data structure for storing metadata of file
// 
// bool fastaccess => 0 for slow access(more secured file) and 1 for fast access
// bool filetype => 0 for file and 1 for directory
struct metadata
{
	bool filetype;
	bool fastaccess;	// for now by default fastaccess is off.
};

// Data structure for storing data block of file
//   
// d_indirectblock => double directed block
// t_indirectblock => triple directed block
struct blockdata
{
	int directblock[MAX_DIRECT_BLOCK];
	int indirectblock;
	int d_indirectblock;
	int t_indirectblock;
};

// Data structure of Inode(for files and directories)
//
// addr => address of inode like 0,1,2
// isallocated => 1 for inode number is allocated to file or dir and 0 for free
struct inode
{
	bool isallocated;
	int addr;	
	struct metadata mdata;
	struct blockdata bdata;
};
