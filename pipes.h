#ifndef PIPES_H
#define PIPES_H

int find_pipes(char* inp_tkns[],int tkn_num, int start);

void handle_pipe(char* largs[],char* rargs[]);

void build_pipe_args(char* pipel_args[] ,char* piper_args[],int svd_tkn_indx, int pipe_i,int tkn_num, char* inpt_tkns[]);

#endif //PIPES_H
