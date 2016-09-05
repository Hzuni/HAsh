#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "redirect.h"

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
       int svd_tkn_indx;
       char* file_name;
       char* args[2000];

       /*Build args and vars for redirection operation*/
       rdrct_i = get_rdrct(inpt_tkns,tkn_num,tkn_indx);
       svd_tkn_indx = tkn_indx;
       if(rdrct_i != -1)
       {
           rdrct_t = rdrct_type(inpt_tkns,rdrct_i);
           file_name = inpt_tkns[(rdrct_i + 1)];

           for(int i = 0; i < (rdrct_i - svd_tkn_indx ); i++)
           {
               args[i] = inpt_tkns[ svd_tkn_indx + i ];
           }
           args[( rdrct_i - svd_tkn_indx ) + 1] = NULL;
       }
       
       /*Creating a new process for the entered line*/
       int rc = fork();
       int stdi_rdrct;
       if(rc < 0)
       {
           fprintf(stderr, "fork failed\n");
           exit(1);
       } else if (rc == 0)
       {
           /*Only will modify args if it's redirect!*/
           if(rdrct_i != -1)
           {
               if(rdrct_t == 0)
               {
                   /*file input redirection*/
                   close(STDIN_FILENO);
                   stdi_rdrct =  open(file_name, O_RDONLY,S_IRWXU);
                   if( stdi_rdrct == -1 )
                   {
                       printf("yash: %s: No such file or directory\n",file_name);
                       exit(0);
                   }

               } else if(rdrct_t == 1)
               {
                   close(STDOUT_FILENO); 
                   open(file_name, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
               } else{
                   /*Redirect standard error*/
                   close(STDERR_FILENO);
                   open(file_name, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
               }

               execvp(args[0], args);
           } else
           {
                execvp(inpt_tkns[0], inpt_tkns);
           }
           printf("this shouldn't be printed"); 

       } else
       {
            int wc = wait(NULL);
       }
    }
    return 0;
       
   
    
}

