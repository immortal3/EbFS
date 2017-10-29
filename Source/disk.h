#ifndef DISK_H
#define DISK_H
/*Reference : https://www3.nd.edu/~dthain/courses/cse30341/spring2017/project6/ */

/* Disk block of size = 4 kb  */
#define DISK_BLOCK_SIZE 4096


/*

 defining methods

    disk_init : intitalizing disk
    disk_size : return size of disk interms of number of blocks
    disk_read : reading block from disk given block number
    disk_write : writing block into disk given block number and data
  	disk_close : closing virtual disk


*/
int  disk_init( const char *filename, int nblocks );
int  disk_size();
void disk_read( int blocknum, char *data );
void disk_write( int blocknum, const char *data );
void disk_close();


#endif
