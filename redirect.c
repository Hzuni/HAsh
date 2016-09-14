#include "redirect.h"
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


int get_rdrct(char* inpt_tkns[], int tkn_num, int start_index)
{
    /*Returns the index of a rdrct tocken*/
    int i;
    int rdrct_index = -1;
    
    for(i = start_index; i < tkn_num; i++)
    {
               if( strcmp(inpt_tkns[i],"<") == 0 )
               {
                   rdrct_index = i;
               }
               else if( strcmp(inpt_tkns[i],">") == 0 )
               {
                   rdrct_index = i;
               }
               else if( strcmp(inpt_tkns[i],"2>") == 0 )
               {
                   rdrct_index = i;
               }
     }
     return rdrct_index; 
}

int rdrct_type(char* inpt_tkns [], int index)
{

   if( strcmp(inpt_tkns[index],"<") == 0 )
   {
       return 0;
   }
   else if( strcmp(inpt_tkns[index],">") == 0 )
   {
       return 1;
   }
   else if( strcmp(inpt_tkns[index],"2>") == 0 )
   {
       return 2;
   }


}
void build_rdrct_args(int rdrct_i, char* args[], char* inpt_tkns[], int svd_tkn_indx)
{
   int i;
   for(i = 0; i < (rdrct_i - svd_tkn_indx ); i++)
   {
       args[i] = inpt_tkns[ svd_tkn_indx + i ];
   }
   args[( rdrct_i - svd_tkn_indx ) + 1] = NULL;
}

void handle_rdrct(char* file_name, int rdrct_t)
{
        int stdi_rdrct; 
        if(rdrct_t == 0)
        {
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
           //Redirect standard error
           close(STDERR_FILENO);
           open(file_name, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
       }
 
}
