#include "inode.h"
#include <stdbool.h>

// Function name : metadata 
// Info : 
// bool filetype : 0 for file and 1 for directory
struct metadata
{
	bool filetype;
};


// 
//   
// d_indirectblock => double directed block
// t_indirectblock => triple directed block
struct blockdata
{
	int[MAX_DIRECT_BLOCK] directblock;
	int indirectblock;
	int d_indirectblock;
	int t_indirectblock;
};

struct inode
{
	struct metadata mdata;
	struct blockdata bdata;
};
