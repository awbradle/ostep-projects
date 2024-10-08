#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void unzip(FILE* in);
int main(int argc, char* argv[])
{	
	FILE* in = NULL;
	
	if(argc < 2)
	{
		fprintf(stdout, "wunzip: file1 [file2 ...]\n");
		exit(1);
	}
	
	//Open input files
	else
	{
		int i;
		for(i = 1; i < argc; i++)
		{
			in = fopen(argv[i], "r");
			if(in == NULL)
			{
				fprintf(stdout, "wunzip: cannot open file\n");
				exit(1);
			}
			unzip(in);
			fclose(in);
		}
	}
}

void unzip(FILE* in)
{
	int i;
	uint32_t count;
	char letter;
	while(fread(&count, sizeof(uint32_t), 1, in) > 0)
	{
		fread(&letter, sizeof(char), 1, in);
		for(i = 0; i < count; i++)
			printf("%c", letter);
	}
}

	
