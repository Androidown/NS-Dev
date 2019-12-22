#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FILE_MAX_SIZE (1024*1024)


void logInfo(char* filename, char* buffer)
{
	unsigned buf_size = strlen(buffer);
	if(filename != NULL && buffer != NULL)
	{
		FILE *fp;
		fp = fopen(filename, "at+");
		if(fp != NULL)
		{
			fwrite(buffer, buf_size, 1, fp);
			fclose(fp);
			fp = NULL;
		}
	}
}
