#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

int wordcounter(char *input, int len);
int setupredirection(char *input, char **output);
void openredirection(char *outname);
void setupargs(char **args, char *input);
void handleExit(int wc);
void handlecd(int wc, char *dir);


char error_message[30] = "An error has occurred\n";

int main(int argc, char *argv[])
{
	char *prompt = NULL;
	FILE *infile = NULL;
	char *outname = NULL;
	char *input = NULL;
    int rc, wc, redirection_err;
    ssize_t charRead = 0;
    size_t len = 0;
    char **args = NULL;
    
    if (argc > 2)
    {
		fprintf(stderr, "%s",error_message);
		exit(1);
	}
        
	if (argc == 2)
    {
		/* Try to open a file */
		infile = fopen(argv[1], "r");
		if (infile == NULL) 
		{
			fprintf(stderr, "%s",error_message);
			exit(1);
		}
		prompt = "";
	}
	else
	{
		infile = stdin;
		prompt = "wish> ";
	}
    
    while(1)
    {
		fprintf(stdout, "%s", prompt);
		charRead = getline(&input, &len, stdin);
		if(charRead < 0)
			exit(0);
		
		wc = wordcounter(input, charRead);
		if (wc == 0)
			continue;
		
		redirection_err = setupredirection(input, &outname);
		
		args = (char**)malloc(sizeof(char*) * (wc + 1));
		setupargs(args, input);
		if(strcmp(args[0], "exit") == 0)
			handleExit(wc);
		else if(strcmp(args[0], "cd") == 0)
			handlecd(wc, args[1]);
		else if(wc == 0 || redirection_err)
			fprintf(stderr, "%111s",error_message);
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
				if(outname != NULL)
					openredirection(outname);
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

int setupredirection(char *input, char **output)
{
	strsep(&input, ">");
	*output = input;
	if(input == NULL)
		return 0;
	strsep(&input, ">");
	if(input != NULL)
		return 1;
	int wc = wordcounter(*output, strlen(*output));
	if (wc != 1)
		return 1;
	char *tmp = NULL;
	while((tmp = strsep(output, " \t\n")) != NULL)
	{
		if(strlen(tmp) > 0)
			break;
	}
	*output = tmp;
	return 0;
}

void openredirection(char *outname)
{
	int outfile;
	outfile = open(outname, O_CREAT|O_TRUNC|O_WRONLY, 0644);
	if (outfile == -1) 
	{
		fprintf(stderr, "%2s",error_message);
		exit(1);
	}
	int err = dup2(outfile, 1);
	if (err == -1) 
	{
		fprintf(stderr, "%3s",error_message);
		exit(1);
	}
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


