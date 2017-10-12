#ifndef DISK_H
#define DISK_H
/*Reference : https://www3.nd.edu/~dthain/courses/cse30341/spring2017/project6/ */

/* Disk block of size = 4 kb  */
#define DISK_BLOCK_SIZE 4096


/* defining methods
    For methods description check .c file
    disk_init : 
    disk_size :
    disk_read :
    disk_write :
  	disk_close :
*/
int  disk_init( const char *filename, int nblocks );
int  disk_size();
void disk_read( int blocknum, char *data );
void disk_write( int blocknum, const char *data );
void disk_close();


#endif
