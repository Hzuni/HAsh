#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "pipes.h"

int find_pipes(char* inpt_tkns[], int tkn_num, int start_index)
{
    
    int i;
    int pipe_index = -1;
    for(i = start_index; i < tkn_num; i++)
    {
        if( strcmp(inpt_tkns[i],"|") == 0 )
        {
            pipe_index = i;
        }
    }
    return pipe_index;
    
            


}

void handle_pipe(char* largs[], char* rargs[])
{
    int pipefd[2];
    if(pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    int pipe_cpid = fork();
    if( pipe_cpid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if(pipe_cpid == 0)
    {
        close(pipefd[1]); //closes the write end of the pipe
        dup2(pipefd[0],STDIN_FILENO);
       //closes STDIN_FILENO and read from the pipe instead
       execvp(rargs[0],rargs);
    } else 
    {
       close(pipefd[0]);//Parent closes read end of pipe
       dup2(pipefd[1], STDOUT_FILENO);
       execvp(rargs[0],rargs);
    }

}

