/* 
 * File:   main.c
 * Author: Nathan Hernandez,
 *         Alyssa Tyler
 * 
 * LoginID: hernandeznp,
 *          tylerae
 *
 * Created on April 2, 2014, 3:08 AM
 */

/**
 * Includes
 */
#include "cachelab.h"
#include "csim.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>

/**
 * Defines
 */
#define BUFFER_SIZE 80

void debugArgs(argument_t *args) {
    printf(
            "args.setBits = %s\n"
            "args.associativity = %s\n"
            "args.blockBits = %s\n"
            "args.traceFile = %s\n",
            args->setBits,
            args->associativity,
            args->blockBits,
            args->traceFile
    );
}

void debugFlags(flag_t *flags) {
    printf(
            "flags.help = %d\n"
            "flags.verbose = %d\n"
            "flags.setBits = %d\n"
            "flags.associativity = %d\n"
            "flags.blockBits = %d\n"
            "flags.traceFile = %d\n",
            flags->help,
            flags->verbose,
            flags->setBits,
            flags->associativity,
            flags->blockBits,
            flags->traceFile
    );
}

void debugCache(Cache *cache) {
    printf(
            "cache.associativity = %s\n"
            "cache.blockBits = %s\n"
            "cache.blockSize = %s\n"
            "cache.setBits = %s\n"
            "cache.setSize = %s\n"
            "cache.tags = 0x%h\n",
            cache->associativity,
            cache->blockBits,
            cache->blockSize,
            cache->setBits,
            cache->setSize,
            cache->tags
    );
}

void debugTrace(trace_t *trace) {
    printf( 
            "trace.address = %d\n"
            "trace.operation = %c\n"
            "trace.size = %d\n",
            trace->address,
            trace->operation,
            trace->size
    );
}

/**
 * Main body - initializes values and controls logic flow.
 * 
 * @param argc number of command-line options
 * @param argv arguments of command-line options
 * @return EXIT_FAILURE or EXIT_SUCCESS depending on runtime conditions
 */
int main(int argc, char * argv[])
{
    flag_t flags;
    flags.help = false;
    flags.verbose = false;
    flags.setBits = false;
    flags.associativity = false;
    flags.blockBits = false;
    flags.traceFile = false;

    argument_t args;	
    args.setBits = NULL;
    args.associativity = NULL;
    args.blockBits = NULL;
    args.traceFile = NULL;

    Cache cache;
    cache.associativity = 0;
    cache.blockBits = 0;
    cache.blockSize = 0;
    cache.setBits = 0;
    cache.setSize = 0;
    cache.tags = NULL;
        
    printf("Reading options...\n");
    
    if(!getOptions(argc, argv, &flags, &args))
            return (EXIT_FAILURE);
    
    printf(
            "Options read.\n"
            "Creating cache...\n"
    );
       
    // We now MUST make sure we free the memory we've dynamically allocated.
    optionsToCache(&args, &cache);
    
    printf(
            "Cache created.\n"
            "Parsing trace file...\n"
    );
    
    if(!readAndParseTraceFile(&args,&cache)) 
    {
        freeCacheTags(&cache);
        return (EXIT_FAILURE);
    }

    printf(
            "Trace file parsed.\n"
            "Freeing cache memory...\n"
    );
    
    freeCacheTags(&cache);

    printf("Cache memory freed.\n");

    
    // pass to printSummary the number of hits, misses and evictions
    //printSummary(0, 0, 0);
    return (EXIT_SUCCESS);
}

/**
 * Creates a Cache based on options and arguments gathered by getOptions().
 * <p>
 * This function dynamically allocates data onto the heap. Be sure to call
 * freeCacheTags() to free memory and avoid memory leaks.
 */
void optionsToCache(argument_t *args, Cache *cache) {
    cache->associativity = atoi(args->associativity);
    cache->blockBits = atoi(args->blockBits);
    cache->blockSize = pow(2,cache->blockBits);
    cache->setBits = atoi(args->setBits);
    cache->setSize = pow(2,cache->setBits);
    cache->tags = (uint64_t **) malloc(cache->setSize*sizeof(uint64_t *));
    uint64_t set;
    for(set=0; set<cache->setSize; set++)
        cache->tags[set] = (uint64_t *) malloc(cache->associativity*sizeof(uint64_t));
    
    uint8_t associativity;
    for(set=0; set<cache->setSize; set++)
        for(associativity=0; associativity<cache->associativity; associativity++)
            cache->tags[set][associativity]=-1;
}

/**
 * Frees the dynamically allocated data from the heap that was created when
 * calling optionsToCache().
 */
void freeCacheTags(Cache *cache) {
    uint64_t set;
    for(set=0; set<cache->setSize; set++)
        free(cache->tags[set]);
    free(cache->tags);
}

/**
 * Prints the program usage to the screen.
 * <p>
 * Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>
 * -h: Optional help flag that prints usage info
 * -v: Optional verbose flag that displays trace info
 * -s <s>: Number of set index bits (S = 2^s is the number of sets)
 * -E <E>: Associativity (number of lines per set)
 * -b <b>: Number of block bits (B = 2^b is the block size)
 * -t <tracefile>: Name of the valgrind trace to replay
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
 * Gathers command-line option data.
 * <p>
 * Values gathered are stored by into the flag_t and argument_t types.
 * 
 * @param argc number of command-line options
 * @param argv arguments of command-line options
 * @return true if all required options and arguments are present with no
 * invalid options
 */
bool getOptions(int argc, char *argv[], flag_t *flags, argument_t *args) {
    extern char *optarg; 
    char option;

    while ((option = getopt (argc, argv, "hvs:E:b:t:")) != -1)
            switch (option)
            {
                    case 'h':
                            flags->help = true;
                            printUsage();
                            return false;
                    case 'v':
                            flags->verbose = true;
                            break;
                    case 's':
                            flags->setBits = true;
                            args->setBits = optarg;
                            break;
                    case 'E':
                            flags->associativity = true;
                            args->associativity = optarg;
                            break;
                    case 'b':
                            flags->blockBits = true;
                            args->blockBits = optarg;
                            break;
                    case 't':
                            flags->traceFile = true;
                            args->traceFile = optarg;
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
                            if (isprint(optopt))
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

    if (!flags->setBits) {	/* -s is missing and mandatory */
            fprintf(stderr, "%s: missing -s option\n", argv[0]);
            printUsage();
            // fprintf(stderr, usage, argv[0]);
            return false;
    }
    if (!flags->associativity) {	/* -E is missing and mandatory */
            fprintf(stderr, "%s: missing -E option\n", argv[0]);
            printUsage();
            // fprintf(stderr, usage, argv[0]);
            return false;
    }
    if (!flags->blockBits) {	/* -b is missing and mandatory */
            fprintf(stderr, "%s: missing -b option\n", argv[0]);
            printUsage();
            // fprintf(stderr, usage, argv[0]);
            return false;
    }
    if (!flags->traceFile) {	/* -t is missing and mandatory */
            fprintf(stderr, "%s: missing -t option\n", argv[0]);
            printUsage();
            // fprintf(stderr, usage, argv[0]);
            return false;
    }
    return true;
}

/**
 * Reads and parses the trace file provided by the argument of the -t command-
 * line option.
 * 
 * @return true if the file is read and parsed without issue
 */
bool readAndParseTraceFile(argument_t *args, Cache *cache) {
    FILE * traceFile;
    char buffer[80];
    
    trace_t trace;
    trace.address = 0;
    trace.operation = 0;
    trace.size = 0;

    traceFile = fopen (args->traceFile , "r");
    
    if (traceFile == NULL) {
        perror ("Error opening file");
        return false;
    }

    printf("File read!\n");
    while ( fgets (buffer , BUFFER_SIZE , traceFile) != NULL ) {
        printf("\n%s",buffer);
        sscanf(buffer,"%*c%c %Lx,%c",
                &(trace.operation),
                &(trace.address),
                &(trace.size)
        );
        // Quick char to int subtraction technique.
        // Equivalent to size = atoi((char *)(&size)) or size = size - 48.
        trace.size -= '0';

        switch(trace.operation) {
            case 'L':
                cacheLoad(&trace,cache);
                break;
            case 'S':
                cacheStore(&trace,cache);
                break;
            case 'M':
                cacheModify(&trace,cache);
                break;
            case 'I':
                break;
            default:
                return false;
        }
    }
    fclose (traceFile);
    
    return true;
}

/**
 * Loads data from the cache.
 * 
 * @param trace valgrind data to be used
 * @param cache the cache to load from
 * @return true on success
 */
bool cacheLoad(trace_t *trace, Cache *cache) {
    if(trace->operation != 'L')
        return false;
    
    return true;
}

/**
 * Stores data into the cache.
 * 
 * @param trace valgrind  data to be used
 * @param cache the cache to store into
 * @return true on success
 */
bool cacheStore(trace_t *trace, Cache *cache) {
    if(trace->operation != 'S')
        return false;
    
    return true;
}

/**
 * Modifies data in the cache.
 * 
 * @param trace valgrind  data to be used
 * @param cache the cache to modify
 * @return true on success
 */
bool cacheModify(trace_t *trace, Cache *cache) {
    if(trace->operation != 'M')
        return false;
    
    return true;
}

/**
 * Returns the bits between start and end.
 * 
 * @param value the value to pull bits from.
 * @param start the start bit (0-63)
 * @param end the end bit (0-63)
 * @return the bits between start and end of value
 */
uint64_t getBits(uint64_t value, uint8_t start, uint8_t end) {
    if(start<0 || start>=64 || end<0 || end>=64 || start>end)
        return 0;
    int i;
    uint64_t mask=0;
    for(i=0; i<=end; i++)
        mask = (mask<<1)|1;
    mask<<=start;
    return mask&value;
}