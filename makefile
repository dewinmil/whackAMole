make: arguments.c mole.c
		gcc -std=c99 -c arguments.c
		gcc -std=c99 -D _BSD_SOURCE -D _XOPEN_SOURCE -Wall mole.c arguments.o
