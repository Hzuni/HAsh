#ifndef REDIRECT_H
#define REDIRECT_H

int get_rdrct(char* inpt_tkns[] ,int tkn_num ,int start_index );

int rdrct_type(char* inpt_tkns[] ,int index );

void handle_rdrct(char* file_name,int rdrct_t);

void build_rdrct_args(int rdrct_i,char* args[],char* inpt_tkns[],int svd_index);




#endif/*REDIRECT_H*/

