#include "cachelab.h"
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

/**
 * Cache definition
 */
typedef struct Cache{ 
	uint8_t sets; 
	uint8_t iBits; 
	uint8_t assoc; 
	uint8_t bBits; 
	uint8_t block; 
	uint8_t ** tags; 
} Cache;

/**
 * Flag type definition
 */
typedef union flag_t{
		struct {
			bool h : 1;
			bool v : 1;
			bool s : 1;
			bool E : 1;
			bool b : 1;
			bool t : 1;
		};
		uint8_t raw;
}flag_t;

/**
 * Argument type definition
 */
typedef struct argument_t {
	char * s;
	char * E;
	char * b;
	char * t;
} argument_t;

/**
 * Function signatures
 */
void printUsage(void);
bool getArgs(int argc, char *argv[]);

/**
 * Static variables
 */
static flag_t flags;
static argument_t args;

/**
 * Main
 */
int main(int argc, char * argv[])
{
	flags.h = false;
	flags.v = false;
	flags.s = false;
	flags.E = false;
	flags.b = false;
	flags.t = false;
	
	args.s = NULL;
	args.E = NULL;
	args.b = NULL;
	args.t = NULL;
	
	if(!getArgs(argc, argv))
		return 1;

	FILE * pFile;
	char mystring [100];

	pFile = fopen (args.t , "r");
	if (pFile == NULL) 
		perror ("Error opening file");
	else {
		printf("File read!\n");
		if ( fgets (mystring , 100 , pFile) != NULL )
			puts (mystring);
		fclose (pFile);
   }
	
    // pass to printSummary the number of hits, misses and evictions
    //printSummary(0, 0, 0);
    return 0;
}

/**
 *
 */
void printUsage(void){
        printf(
			"\nUsage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n"
			"\t-h: Optional help flag that prints usage info\n"
			"\t-v: Optional verbose flag that displays trace info\n"
			"\t-s <s>: Number of set index bits (S = 2^s is the number of sets)\n"
			"\t-E <E>: Associativity (number of lines per set)\n"
			"\t-b <b>: Number of block bits (B = 2^b is the block size)\n"
			"\t-t <tracefile>: Name of the valgrind trace to replay\n"
		);
}

/**
 *
 */
bool getArgs(int argc, char *argv[]) {
	static char usage[] = "Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n";

	extern char *optarg; 
	char option;
	
	while ((option = getopt (argc, argv, "hvs:E:b:t:")) != -1)
		switch (option)
		{
			case 'h':
				flags.h = true;
				printUsage();
				return false;
			case 'v':
				flags.v = true;
				break;
			case 's':
				flags.s = true;
				args.s = optarg;
				break;
			case 'E':
				flags.E = true;
				args.E = optarg;
				break;
			case 'b':
				flags.b = true;
				args.b = optarg;
				break;
			case 't':
				flags.t = true;
				args.t = optarg;
				break;
			case '?':
				switch(optopt) {
					case 's':
					case 'E':
					case 'b':
					case 't':
						// fprintf (stderr, "Option -%c requires an argument.\n", optopt);
						printUsage();
						return false;
				}
				if (isprint (optopt))
				{
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
					printUsage();
				}
				else
				{
					fprintf (stderr,
					"Unknown option character `\\x%x'.\n",
					optopt);
					printUsage();
				}
				return false;
			default:
				return false;
		}
	
	if (!flags.s) {	/* -s is missing and mandatory */
		fprintf(stderr, "%s: missing -s option\n", argv[0]);
		printUsage();
		// fprintf(stderr, usage, argv[0]);
		return false;
	}
	if (!flags.E) {	/* -E is missing and mandatory */
		fprintf(stderr, "%s: missing -E option\n", argv[0]);
		printUsage();
		// fprintf(stderr, usage, argv[0]);
		return false;
	}
	if (!flags.b) {	/* -b is missing and mandatory */
		fprintf(stderr, "%s: missing -b option\n", argv[0]);
		printUsage();
		// fprintf(stderr, usage, argv[0]);
		return false;
	}
	if (!flags.t) {	/* -t is missing and mandatory */
		fprintf(stderr, "%s: missing -t option\n", argv[0]);
		printUsage();
		// fprintf(stderr, usage, argv[0]);
		return false;
	}
	return true;
}