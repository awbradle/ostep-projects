#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t count = 0;
char nextLetter;
char currentLletter;

void zip(FILE* in);
int main(int argc, char* argv[])
{	
	FILE* in = NULL;
	
	if(argc < 2)
	{
		fprintf(stdout, "wzip: file1 [file2 ...]\n");
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
				fprintf(stdout, "wzip: cannot open file\n");
				exit(1);
			}
			zip(in);
			fclose(in);
		}
		if(count > 0)
		{
			fwrite(&count, sizeof(uint32_t), 1, stdout);
			fwrite(&currentLletter, sizeof(char), 1, stdout);
		}
	}
}

//Handle reading a single file. Note char count can span multiple files
void zip(FILE* in)
{
	if (count == 0)
	{
		if(fread(&nextLetter, sizeof(char), 1, in) == 0)
			return;
		currentLletter = nextLetter;
		count = 1;
	}
	while(fread(&nextLetter, sizeof(char), 1, in) > 0)
	{
		if(nextLetter != currentLletter)
		{
			fwrite(&count, sizeof(uint32_t), 1, stdout);
			fwrite(&currentLletter, sizeof(char), 1, stdout);
			currentLletter = nextLetter;
			count = 1;
		}
		else
			count++;
	}
}