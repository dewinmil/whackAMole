/*
 * Created by Miles De Wind - 02/22/2017
 * Utilized code provided by Ira Woodring
 */

#include <stdlib.h>
#include "arguments.h"

// Program title and version
const char *argp_program_version = "mole_args 1.0";
// Who to contact if bugs are found
const char *argp_program_bug_address = "dewinmil@mail.gvsu.edu";

// Documentation string.
char doc[] = "Demonstrate Args - a program to demonstrate how to use argp to parse command-line agruments.";

// Description of the required arguments we accept.
char args_doc[] = "";

// This option struct is for any optional arguments.
struct argp_option options[] = {
	{"width",	'w',	"WIDTH",	0,	"The width of the board - the horizontal number of cells, default 3." },
	{"height",	'h',	"HEIGHT",	0,	"Sets the height of the board - the vertical number of cells, default 3." },
	{"max", 	'm',	"MAX",  	0,	"Sets the max number of moles that will poke their heads out at once." },
	{"total",	't',	"TOTAL",	0,	"Sets the total number of moles in the game." },
	{"up",  	'u',	"UP",		0,	"Sets th time the moles pop their heads out of their holes (in milliseconds)." },
	{"down",	'd',	"DOWN", 	0, 	"Sets the time the moles stay in their holes (in milliseconds)" },
	{ 0 }
};

// Argp requires us to pass these values in via the argp struct
struct argp argp = { options, parse_opt, args_doc, doc };
// Declare a variable called arguments that is also an
// arguments struct.  Remeber - we can't use the struct
// by just describing it.  We also need to make one.  Sort
// of similar to how we instantiate an object in Java.
struct arguments arguments;

// Our custom function for setting up values once they
// have been parsed from the command line args.
error_t parse_opt(int key, char* arg, struct argp_state *state){
	struct arguments *arguments = state->input;
	switch(key){
		case 'w':
			arguments->width = atoi(arg);
			break;
		case 'h':
			arguments->height = atoi(arg);
			break;
		case 'm':
			arguments->max = atoi(arg);
			break;
		case 't':
			arguments->total = atoi(arg);
			break;
		case 'u':
			arguments->up = atoi(arg);
			break;
		case 'd':
			arguments->down = atoi(arg);
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

// Here we setup some default values and override them
// if we get new values from the commandline.
struct arguments setup(int argc, char** argv){
	// Default of first, second, and load_file
	int width = 3;
	int height = 3;
        int max = 3;
        int total = 40;
        int up = 2500;
        int down = 6000;

	argp_parse(&argp, argc, argv, 0, 0, &arguments);
	// Return values are in the arguments struct.  Pull
	// them out if they are there.
	if(arguments.width != 0){
	  width = arguments.width;
	}
	if(arguments.height != 0){
	  height = arguments.height;
	}
	if(arguments.max != 0){
    	  max = arguments.max;
	}
	if(arguments.total != 0){
    	  total = arguments.total;
	}
	if(arguments.up != 0){
    	  up = arguments.up;
	}
	if(arguments.down != 0){
    	  down = arguments.down;
	}
        if(max > (width * height)){
	  width = -1;
	  height = -1;
	  max = -1;
	}
	arguments.width = width;
	arguments.height = height;
	arguments.max = max;
	arguments.total = total;
	arguments.up = up;
	arguments.down = down;
	return arguments;
}
