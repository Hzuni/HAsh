#include "redirect.h"
#include <string.h>

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
