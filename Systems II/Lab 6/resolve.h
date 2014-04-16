/* 
 * File:   resolve.h
 * Author: Nathan Hernandez 
 *
 * Created on April 16, 2014, 6:41 AM
 */

#ifndef RESOLVE_H 
#define	RESOLVE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdbool.h>

static bool isArchive(char * filename);
static bool isObjectFile(char * filename);
static void handleObjectFile(char * filename, linkedList_t * defined, linkedList_t * undefined);
static void handleArchive(char * filename, linkedList_t * defined, linkedList_t * undefined);
static void displayMessageAndExit(char * message);

#ifdef	__cplusplus
}
#endif

#endif	/* RESOLVE_H */

