#ifndef BUILTINS_H
#define BUILTINS_H 

int hash_cd(char **args);
int hash_help(char **args);
int hash_exit(char **args);
int hash_num_builtins();
extern char *builtin_str[];
extern int (*builtin_func[]) (char **);

#endif //BUILTINS_H



 
