#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "redirect.h"
#include "pipes.h"

int main(int argc, const char* argv[])
{
    while(1)
    {
       
       printf("#");
       char* inpt_str = malloc(2000);
       char* inpt_tkns[2000];
       char* tkn_ptr;   
       int tkn_num = 0; 
      
       fgets(inpt_str,2000,stdin);
       tkn_ptr = strtok(inpt_str," \n");
       if( tkn_ptr == NULL)
       {
           /* If nothing is entered*/
           continue;
       }

       while(tkn_ptr != NULL)
       {
           inpt_tkns[tkn_num] = tkn_ptr;
           tkn_ptr = strtok(NULL," \n");
           tkn_num +=1;
       }
       inpt_tkns[tkn_num] = NULL;
       
       int tkn_indx = 0;
       int rdrct_i;
       int rdrct_t;
       int pipe_i;

       int svd_tkn_indx;
       char* file_name;
       char* args[2000];
       char* pipel_args[2000];
       char* piper_args[2000];
       int i;

       /*Build args and vars for redirection operation*/
       rdrct_i = get_rdrct(inpt_tkns,tkn_num,tkn_indx);
       svd_tkn_indx = tkn_indx;
       pipe_i = find_pipes(inpt_tkns,tkn_num,tkn_indx);
       
       if(rdrct_i != -1)
       {
           rdrct_t = rdrct_type(inpt_tkns,rdrct_i);
           file_name = inpt_tkns[(rdrct_i + 1)];

           for(i = 0; i < (rdrct_i - svd_tkn_indx ); i++)
           {
               args[i] = inpt_tkns[ svd_tkn_indx + i ];
           }
           args[( rdrct_i - svd_tkn_indx ) + 1] = NULL;
       }
       if(pipe_i != -1)
       {
           for(i = 0; i < (pipe_i - svd_tkn_indx); i++)
           {
               pipel_args[i] = inpt_tkns[svd_tkn_indx + i];
           }
           
           for(i = 0; i < (tkn_num - (pipe_i +1)); i++)
           {
               piper_args[i] = inpt_tkns[(pipe_i + 1) + i];
           }

       }
       
       /*Creating a new process for the entered line*/
       int rc = fork();
       
       if(rc < 0)
       {
           fprintf(stderr, "fork failed\n");
           exit(1);
       } else if (rc == 0)
       {
           if(rdrct_i != -1)
           {
               handle_rdrct(file_name, rdrct_t);
               execvp(args[0], args);

           }               
           execvp(inpt_tkns[0], inpt_tkns);   
       }else
       {
           if(pipe_i != -1)
           {


           }
           int wc = wait(NULL);
            
       }
    }
    return 0;
       
   
    
}

