hash: hash.c
	gcc -g hash.c redirect.c pipes.c jobs.c builtins.c -o hash

debug:
	gdb hash
