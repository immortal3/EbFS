

void printing_util()
{
	printf("\n--------------New Operation---------------\n");
}

char *slice_array(char *array, int start, int end) 
{
    int numElements = (end - start + 1);
    int numBytes = sizeof(char) * numElements;

    char *slice = (char *)malloc(numBytes);
    memcpy(slice, array + start, numBytes);
    return slice;
}


char *read_Whole_file(char filename[])
{
	FILE *fp;
	long lSize;
	char *buffer;

	fp = fopen ( filename , "rb" );
	if( !fp ) perror(filename),exit(1);

	fseek( fp , 0L , SEEK_END);
	lSize = ftell( fp );
	rewind( fp );

	/* allocate memory for entire content */
	buffer = (char *)calloc( 1, lSize+1 );
	if( !buffer ) fclose(fp),fputs("memory alloc fails",stderr),exit(1);

	/* copy the file into the buffer */
	if( 1!=fread( buffer , lSize, 1 , fp) )
	  fclose(fp),free(buffer),fputs("entire read fails",stderr),exit(1);

	/* do your work here, buffer is a string contains the whole text */

	fclose(fp);
	return buffer;
}