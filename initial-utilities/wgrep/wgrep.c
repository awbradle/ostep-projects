#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void grep(char* search, FILE* in);

int main(int argc, char* argv[])
{
	FILE *in = stdin;
	char* search = NULL;
	
	if(argc < 2)
	{
		fprintf(stdout, "wgrep: searchterm [file ...]\n");
		exit(1);
	}
	//Set search term
	search = argv[1];
	
	//Handle STDIN
	if(argc == 2)
	{
		grep(search, in);
	}
	
	//Open input files
	if(argc > 2)
	{
		int i;
		for(i = 2; i < argc; i++)
		{
			in = fopen(argv[i], "r");
			if(in == NULL)
			{
				fprintf(stdout, "wgrep: cannot open file\n");
				exit(1);
			}
			grep(search, in);
		}
		fclose(in);
	}
}
	
void grep(char* search, FILE* in)
{
	char *line = NULL;
	unsigned long len = 0;
	
	//Read file line by line, print if search term found in line
	while(getline(&line, &len, in) > 0)
	{
		if (strstr(line, search) != NULL)
			printf("%s", line);
	}
}
	
