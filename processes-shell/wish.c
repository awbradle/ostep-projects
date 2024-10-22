#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

int wordcounter(char *input, int len);
void setupargs(char **args, char *input);
void handleExit(int wc);
void handlecd(int wc, char *dir);


char error_message[30] = "An error has occurred\n";

int main(int argc, char *argv[])
{

    //printf("hello world (pid:%d)\n", (int) getpid());
    //ssize_t getline(char ** restrict linep, size_t * restrict linecapp,  FILE * restrict stream);
    //char *strsep(char **stringp, const char *delim);
    char *input = NULL;
    int rc, wc;
    ssize_t charRead = 0;
    size_t len = 0;
    char **args = NULL;
    while(1)
    {
		printf("wish>");
		charRead = getline(&input, &len, stdin);
		if(charRead < 0)
			exit(0);
		
		wc = wordcounter(input, charRead);
		printf("words counted %d\n", wc);
		if (wc == 0)
			continue;
		
		args = (char**)malloc(sizeof(char*) * (wc + 1));
		setupargs(args, input);
		if(strcmp(args[0], "exit") == 0)
			handleExit(wc);
		else if(strcmp(args[0], "cd") == 0)
			handlecd(wc, args[1]);
		else
		{
			
			rc = fork();
			if (rc < 0) 
			{
				// fork failed; exit
				fprintf(stderr, "fork failed\n");
				exit(1);
			} 
			else if (rc == 0) 
			{
				execvp(args[0], args);  // runs word count
				fprintf(stderr, "%s",error_message);
				exit(1);
			} 
			else 
			{
				// parent goes down this path (original process)
				waitpid(rc, NULL, 0);
			}
		}
    	free(args);
    }
    return 0;
}

int wordcounter(char *input, int len)
{
	int wc = 0;
	int i;
	int in = 0;
	for(i = 0; i < len; i++)
	{
		if(in && isspace(input[i]))
		{
			in = 0;
		}
		else if((!in) && (!isspace(input[i])))
		{
			in = 1;
			wc++;
		}
	}
	return wc;
}
void setupargs(char **args, char *input)
{
	int i = 0;
	char* tmp = NULL;
	while((tmp = strsep(&input, " \t\n")) != NULL)
	{
		if(strlen(tmp) > 0)
			args[i++] = tmp;
	}
	args[i] = NULL;
	printf("args counted %d\n", i);
}

void handleExit(int wc)
{
	if(wc == 1)
		exit(0);
	else
		fprintf(stderr, "%s",error_message);
	
}

void handlecd(int wc, char *dir)
{
	int err = 0;
	if(wc == 2)
		err = chdir(dir);
	if(err != 0 || wc != 2)
		fprintf(stderr, "%s",error_message);
}


