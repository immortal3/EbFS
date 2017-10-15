#include "inode.h"
#include <stdbool.h>

// struct name : metadata 
// Info : 
// bool filetype : 0 for file and 1 for directory
// protection : just like linux ( rwx-rwx-rwx )
struct metadata
{
	bool filetype;
	bool isvalid;
	unsigned char protection[3];
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
	struct metadata mdata;
	struct blockdata bdata;
};
