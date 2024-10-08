#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

struct list 
{
	struct list *next;
	char *line;
};

int main(int argc, char* argv[])
{
	FILE *in = stdin;
	FILE *out = stdout;
	char *line = NULL;
	char *linecpy = NULL;
	unsigned long len = 0;
	struct list *head = NULL;
	struct list *next = NULL;
	if(argc > 3)
	{
		fprintf(stderr, "usage: reverse <input> <output>\n");
		exit(1);
	}
	
	//Open Input File
	if(argc >= 2)
	{
		//printf("2 arg: %s %s\n",argv[0], argv[1]);
		in = fopen(argv[1], "r");
		if(in == NULL)
		{
			fprintf(stderr, "reverse: cannot open file '%s'\n",argv[1]);
			exit(1);
		}
	
	}
	
	//Open Output File
	if(argc >= 3)
	{
		//printf("3 arg: %s %s %s\n",argv[0], argv[1], argv[2]);
		out = fopen(argv[2], "w");
		if(out == NULL)
		{
			fprintf(stderr, "reverse: cannot open file '%s'\n",argv[2]);
			exit(1);
		}
		if (strcmp(argv[1],argv[2]) == 0)
		{
			fprintf(stderr, "reverse: input and output file must differ\n");
			exit(1);
		}
		struct stat instat;
		stat(argv[1],&instat);
		struct stat outstat;
		stat(argv[2],&outstat);
		if(instat.st_ino == outstat.st_ino)
		{
			fprintf(stderr, "reverse: input and output file must differ\n");
			exit(1);
		}
	}
	
	while((len = getline(&line, &len, in) > 0))
	{
		linecpy = (char*) malloc(sizeof(char) * (strlen(line) + 1));
		next = (struct list*) malloc(sizeof(struct list));
		if(linecpy == NULL || next == NULL)
		{
			fprintf(stderr, "reverse: malloc failed\n");
			exit(1);
		}
		strcpy(linecpy, line);
		next->line = linecpy;
		next->next = head;
		head = next;
	}
	
	//print out reverse
	while(head != NULL)
	{
		fprintf(out, "%s", head->line);
		free(head->line);
		next = head;
		head = head->next;
		free(next);
	}
	fclose(in);
	fclose(out);
	return 0;
}