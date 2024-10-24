#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/wait.h>

int wordcounter(char *input, int len);
int setupredirection(char *input, char **output);
void openredirection(char *outname);
void setupargs(char **args, char *input);
void handleExit(int wc);
void handlecd(int wc, char *dir);
void handlepath(int wc, char **args);
char* getAccessPath(char *prog);
 
char error_message[30] = "An error has occurred\n";
char **path = NULL;

int main(int argc, char *argv[])
{
	FILE *infile = NULL;
	int batchMode = 0;
	char *outname = NULL;
	char *input = NULL;
	char *inputPiece = NULL;
    int rc, wc, redirectionErr;
    int pid = 0;
    ssize_t charRead = 0;
    size_t len = 0;
    char **args = NULL;
    
    path = (char**) malloc(sizeof(char*) * 2);
    path[0] = (char*) malloc(sizeof(char) * 5);
    snprintf(path[0],sizeof(path[0]), "/bin");
    path[1] = NULL;
    rc = -1;
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
		batchMode = 1;
	}
	else
	{
		infile = stdin;
	}
    
    while(1)
    {
		if(!batchMode)
			fprintf(stdout, "wish> ");
		charRead = getline(&input, &len, infile);
		if(charRead < 0)
			exit(0);
			
		inputPiece = input;
		while(strchr(input, '&') != NULL)
		{
			inputPiece = strsep(&input, "&");
			rc = fork();
			if (rc < 0) 
			{
				// fork failed; exit
				fprintf(stderr, "fork failed\n");
				exit(1);
			}
			if(rc == 0)
				break;
		}
		if (rc != 0)
			inputPiece = strsep(&input, "&");
		wc = wordcounter(inputPiece, strlen(inputPiece));
		if (wc == 0)
		{
			if(rc == 0)
				exit(0);
			pid = 0;
			while(pid != -1)
			{
				pid = wait(NULL);
			}
			continue;
		}
		
		redirectionErr = setupredirection(inputPiece, &outname);
		
		args = (char**)malloc(sizeof(char*) * (wc + 1));
		setupargs(args, inputPiece);
		
		if(redirectionErr || args[0] == NULL)
			fprintf(stderr, "%s",error_message);
		else if(strcmp(args[0], "exit") == 0)
			handleExit(wc);
		else if(strcmp(args[0], "cd") == 0)
			handlecd(wc, args[1]);
		else if(strcmp(args[0], "path") == 0)
			handlepath(wc, args);
		else
		{
			if (rc != 0)
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
				char *progPath = getAccessPath(args[0]);
				if(progPath == NULL)
				{
					fprintf(stderr, "%s",error_message);
					exit(1);
				}
				execvp(progPath, args);  // runs word count
				fprintf(stderr, "%s",error_message);
				exit(1);
			} 
			else 
			{
				// parent goes down this path (original process)
				pid = 0;
				while(pid != -1)
				{
					pid = wait(NULL);
				}	
			}
		}
    	free(args);
    	if(rc == 0)
    		exit(0);
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

void handlepath(int wc, char **args)
{
	int i = 0;
	while(path[i] != NULL)
	{
		free(path[i++]);
	}
	free(path);

	path = (char**) malloc(sizeof(char*) * wc);
	i = 0;
	while(args[i+1] != NULL) 
	{
		asprintf(&path[i], "%s", args[i+1]);
		i++;
	}
	path[i] = NULL;
	i = 0;
}

char* getAccessPath(char *prog)
{
	int i = 0;
	char *progPath = NULL;
	if (path[0] == NULL)
		return NULL;
	
	while(path[i] != NULL)
	{
		if(progPath != NULL)
			free(progPath);
		asprintf(&progPath, "%s/%s", path[i], prog);
		if(access(progPath, X_OK) == 0)
			return progPath;
		i++;
	}
	free(progPath);
	return NULL;
}
