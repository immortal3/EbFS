#ifndef FILESYSTEM_H
#define FILESYSTEM_H
int EbFs_create_file(char [],long int , char [],bool );
int EbFs_read_file(int inodenumber);
#define MINIMUM_DISk_SIZE 30
#define MAX_INODE_IN_BLOCK 50
#endif