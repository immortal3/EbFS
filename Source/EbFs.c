#include <stdio.h>
#include "disk.c"


int main()
{
	// creating virtual disk of 500 blocks 
	// total size = 500 * 4 kb
	int disk_status = disk_init("Disk_img_500",500);
	printf("Disk status : %d\n",disk_status );
}

