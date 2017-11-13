# EbFS
Encryption based File system

## Inode structure:
  

## Technical Specification(Data Structures):

Block size of disk : 4096-byte (4 KiB) (which are used by Newer HDDs)

Data Structures:

1. Superblock:

    Attributes of superblock:
    Size of disk
    Information of Inode blocks ( total number of inodeblocks , etc..)
    Information of bitmap blocks ( starting of bit map blocks , etc.. )
    Total Size of Superblock: 28 bytes
    Location in source code:superblock.c

2. Inode:

    Attributes of inodes:
    Metadata (contains information of filetype, etc.. )
    Blockdata (direct blocks ,indirect blocks , etc..)
    And other information containing flag like is Allocated.

    Total Size of Inode : 72 bytes
    Maximum Number of inodes in per block : 50 inodes per block
    Location in source code: inode.c and inode.h 

3. File_entry:

    Attributes of File_entry:
    File name
    Inode number

    Total size of File_entry : 32 bytes [ 28 bytes of file-name + 4 bytes of inode number ].
    Maximum number of File_entry in each block : 128 
    Location in source code:filesystem.c



4. Union block_rw:

    Attributes of block_rw:
    Char array of size 4096 bytes
    Super block
    Array of inode block
    Array of file entries
    Size of block_rw : Block size of disk ( 4096 bytes)
    Location in source code:filesystem.c
    Disk can not be read and write single bit and byte.We have to read and write in terms of block size of disk.That’s why we      
    used Union block_rw for reading and writing data into disk.



#Assumption :

- We are using Disk emulator which has no IO scheduling algorithms. 


