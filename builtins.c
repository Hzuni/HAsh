#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "builtins.h"



char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &hash_cd,
  &hash_help,
  &hash_exit
};

int hash_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}


int hash_cd(char **args) {
  
  if (args[1] == NULL) {
    fprintf(stderr, "hash: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("hash");
    }
  }

  return 1;

}

int hash_help(char **args) {
	
	int i;
	printf("Hasun Amarasekara's HAsh\n");
	printf("The following built ins are suppourted:\n");

	for (i = 0; i < hash_num_builtins(); i++) {
	    printf(" %s\n", builtin_str[i]);
	}

	printf("Use the man command for information on other programs.\n");
	return 1;

}

int hash_exit(char **args) {
    
    exit(0); 
    return 0;

}
