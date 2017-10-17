#include "inode.h"
#include <stdbool.h>
#include <time.h>

// struct name : metadata 
// Info : 
// bool filetype : 0 for file and 1 for directory
struct metadata
{
	bool filetype;
	bool fastaccess;
};


// 
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

struct inode
{
	bool isallocated;
	int inode_number; // number of inode
	struct metadata mdata;
	struct blockdata bdata;
};
