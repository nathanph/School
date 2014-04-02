/* 
 * File:   csim.h
 * Author: Nathan Hernandez,
 *         Alyssa Tyler
 * 
 * LoginID: hernandeznp,
 *          tylerae
 *
 * Created on April 2, 2014, 3:32 AM
 */


#ifndef CSIM_H  
#define CSIM_H

#include <inttypes.h>
#include <stdbool.h>

/**
 * Cache definition
 */
typedef struct Cache{
    uint8_t associativity; 
    uint8_t blockBits; 
    uint32_t blockSize; 
    uint8_t setBits; 
    uint32_t setSize; 
    uint64_t ** tags; 
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
    struct {
        bool help : 1;
        bool verbose : 1;
        bool setBits : 1;
        bool associativity : 1;
        bool blockBits : 1;
        bool traceFile : 1;
    };
    uint8_t raw;
}flag_t;

/**
 * Argument type definition
 */
typedef union argument_t {
    struct {
        char * s;
        char * E;
        char * b;
        char * t;
    };
    struct {
        char * setBits;
        char * associativity;
        char * blockBits;
        char * traceFile;
    };
} argument_t;

/**
 * Prints the program usage to the screen.
 */
void printUsage(void);

/**
 * Gathers command-line option data.
 * 
 * @param argc number of command-line options
 * @param argv arguments of command-line options
 * @return true if all required options and arguments are present with no
 * invalid options
 */
bool getOptions(int argc, char *argv[], flag_t flags, argument_t args);

/**
 * Creates a Cache based on options and arguments gathered by getOptions().
 * <p>
 * This function dynamically allocates data onto the heap. Be sure to call
 * freeCacheTags() to free memory and avoid memory leaks.
 */
void optionsToCache(argument_t args, Cache cache);

/**
 * Frees the dynamically allocated data from the heap that was created when
 * calling optionsToCache().
 */
void freeCacheTags(Cache cache);

/**
 * Reads and parses the trace file provided by the argument of the -t command-
 * line option.
 * 
 * @return true if the file is read and parsed without issue
 */
bool readAndParseTraceFile(argument_t args, Cache cache);

#endif  /* CSIM_H */