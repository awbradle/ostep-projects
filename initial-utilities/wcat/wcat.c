#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
	char *line = NULL;
	size_t linecap = 0;
	ssize_t len;
	/* quit if no files passed */
	if (argc < 2)
		return 0;
		
	line = (char*) malloc(100);
	int i;
	
	for(i = 1; i < argc; i++)
	{
		/* Try to open a file */
		FILE *fp = fopen(argv[i], "r");
		if (fp == NULL) 
		{
    		printf("wcat: cannot open file\n");
    		exit(1);
		}
		while ((len = getline(&line, &linecap, fp)) > 0)
			fwrite(line, 1, len, stdout);
		fclose(fp);
	}
	free(line);
	return 0;
}
