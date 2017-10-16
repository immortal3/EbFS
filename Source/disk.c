#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "disk.h"
/*Reference : https://www3.nd.edu/~dthain/courses/cse30341/spring2017/project6/ */
#define DISK_MAGIC 0xdeadbeef

static FILE *diskfile;
static int nblocks=0;
static int nreads=0;
static int nwrites=0;


/* creating or loading file which will work as disk */
int disk_init( const char *filename, int n )
{
	diskfile = fopen(filename,"r+");
	if(!diskfile) diskfile = fopen(filename,"w+");
	if(!diskfile) return 0;

	ftruncate(fileno(diskfile),n*DISK_BLOCK_SIZE);

	nblocks = n;
	nreads = 0;
	nwrites = 0;

	return 1;
}


/* will return total number of block (block size  = 4kb ) */
int disk_size()
{
	return nblocks;
}


/* checking number of bad blocks given block number   */
static void sanity_check( int blocknum, const void *data )
{
	if(blocknum<0) {
		printf("ERROR: blocknum (%d) is negative!\n",blocknum);
		abort();
	}

	if(blocknum>=nblocks) {
		printf("ERROR: blocknum (%d) is too big!\n",blocknum);
		abort();
	}

	if(!data) {
		printf("ERROR: null data pointer!\n");
		abort();
	}
}


/* disk_read will read data given block number */

void disk_read(int blocknum, char *data )
{
	sanity_check(blocknum,data);

	fseek(diskfile,blocknum*DISK_BLOCK_SIZE,SEEK_SET);

	if(fread(data,DISK_BLOCK_SIZE,1,diskfile)==1) {
		nreads++;
	} else {
		printf("ERROR: couldn't access simulated disk: %s\n",strerror(errno));
		abort();
	}
}



void disk_write( int blocknum, const char *data )
{
	sanity_check(blocknum,data);

	fseek(diskfile,blocknum*DISK_BLOCK_SIZE,SEEK_SET);

	if(fwrite(data,DISK_BLOCK_SIZE,1,diskfile)==1) {
		nwrites++;
	} else {
		printf("ERROR: couldn't access simulated disk: %s\n",strerror(errno));
		abort();
	}
}


/* closing virtual disk based on file  */
void disk_close()
{
	if(diskfile)
  {

  		printf("\n******************************************\nclosing Disk\n");
  		printf("Summary of Action on disk:\n");
		printf("%d disk block reads\n",nreads);
		printf("%d disk block writes\n",nwrites);
		fclose(diskfile);
		diskfile = 0;
	}
}
