/* 
 * File:   SymbolList.h
 * Author: Nathan Hernandez 
 *
 * Created on April 16, 2014, 6:41 AM
 */

#ifndef SYMBOLLIST_H
#define	SYMBOLLIST_H
 
#ifdef	__cplusplus
extern "C" {
#endif

#include <stdbool.h>
 
typedef struct node_t {
	char type;
	char name[31];
	struct node_t *next;
	struct node_t *previous;
} node_t;

typedef struct linkedList_t {
	node_t *head;
	node_t *tail;
	node_t *current;
} linkedList_t;


/**
 * Creates a linked list.
 *
 * @return a linked list
 */
linkedList_t * initializeList();

/**
 * Frees memory allocated to a linked list.
 *
 * @param list the list to delete
 * @return true on success
 */
bool deleteList(linkedList_t *list);

/**
 * Inserts a new node into the list.
 *
 * @param name the name of the symbol being inserted
 * @param type the type of the symbol being inserted
 * @return true on success
 */
bool insertNode(linkedList_t *list, char * name, char type);

/**
 * Updates a node in the list.
 *
 * @param name the name of the symbol being updated
 * @param type the type of the symbol being updated
 * @return true on success
 */
bool updateNode(linkedList_t *list, char * name, char type);


/**
 * Searches for and returns a node in the list.
 *
 * @param name the name of the symbol to be searched
 * @return the node if found; else NULL
 */
node_t * searchList(linkedList_t *list, char * name);

/**
 * Removes a node from the list.
 *
 * @param name the name of the node to be removed
 * @return true on success
 */
bool removeNode(linkedList_t *list, char * name);


/**
 * Prints the list for debugging.
 *
 * @param list a list to be displayed
 */
void printList( linkedList_t *list );

/**
 * Prints a node for debugging.
 *
 * @param list a node to be displayed
 */
void printNode( node_t *node );

#ifdef	__cplusplus
}
#endif

#endif	/* SYMBOLLIST_H */

