/* 
 * File:   SymbolList.c
 * Author: Nathan Hernandez 
 *
 * Created on April 16, 2014, 6:41 AM
 */ 

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "SymbolList.h"

/**
 * Creates a linked list.
 *
 * @return a linked list
 */
linkedList_t * initializeList()
{
	// Create list
	linkedList_t *list = (linkedList_t *)malloc(sizeof(linkedList_t));
	
	// Declare vars for ease.
	node_t *head;
	node_t *tail;
	node_t *current;
	
	// Allocate memory for head and tail.
	head = (node_t *)malloc(sizeof(node_t));
	tail = (node_t *)malloc(sizeof(node_t));
	
	// Initialize node_t head.
	head->next = tail;
	head->type = '\0';
	head->name[0] = '\0';
	head->previous = NULL;
	
	// Initialize node_t tail.
	tail->previous = head;
	tail->type = '\0';
	tail->name[0] = '\0';
	tail->next = NULL;
	
	// Initialize node_t current.
	current = head;
	
	//Initialize list.
	list->head = head;
	list->tail = tail;
	list->current = current;
	
	return list;
}

/**
 * Frees memory allocated to a linked list.
 *
 * @param list the list to delete
 * @return true on success
 */
bool deleteList(linkedList_t *list) 
{
	// Declare vars for iterating.
	node_t *current = list->head;
	node_t *temp;
	
	// Iterate and remove nodes.
	while( current != NULL ) 
	{
		temp = current;
		current=current->next;
		free(temp);
	}
	
	free(list);
	
	return true;
}

/**
 * Inserts a new node into the list.
 *
 * @param name the name of the symbol being inserted
 * @param type the type of the symbol being inserted
 * @return true on success
 */
bool insertNode(linkedList_t *list, char * name, char type) 
{
	// Declare vars for ease.
	node_t *tail = list->tail;
	
	// Create the new node.
	node_t *newNode = (node_t *)malloc(sizeof(node_t));
	newNode->type = type;
	strcpy(newNode->name,name);
	
	// Insert the new node into the list at the tail.
	newNode->previous = tail->previous;
	newNode->next = tail;
	tail->previous->next = newNode;
	tail->previous = newNode;
	
	return true;
}

/**
 * Updates a node in the list.
 *
 * @param name the name of the symbol being updated
 * @param type the type of the symbol being updated
 * @return true on success
 */
bool updateNode(linkedList_t *list, char * name, char type) 
{
	node_t *node = searchList(list,name);
	
	if(node == NULL) return false;
	
	node->type = type;
	
	return true;
}

/**
 * Searches for and returns a node in the list.
 *
 * @param name the name of the symbol to be searched
 * @return the node if found; else NULL
 */
node_t * searchList(linkedList_t *list, char * name) 
{
	// Declare vars for iterating.
	node_t *current = list->head;
	node_t *needle;
	
	// Iterate nodes until node is found or list ends.
	while( current != NULL ) 
	{
		needle = current;
		
		// If node found return it.
		if(!strcmp(name,needle->name))
		{
			return needle;
		}
		current=current->next;
	}
	
	return NULL;
}

/**
 * Removes a node from the list.
 *
 * @param name the name of the node to be removed
 * @return true on success
 */
bool removeNode(linkedList_t *list, char * name)
{
	// Find the node to remove.
	node_t *node = searchList(list,name);
	
	if(node == NULL) return false;
	
	// Fix its previous and next.
	node_t *previousNode = node->previous;
	node_t *nextNode = node->next;
	
	previousNode->next = node->next;
	nextNode->previous = node->previous;
	
	// Free its memory.
	free(node);
	
	return true;
}

/**
 * Prints the list for debugging.
 *
 * @param list a list to be displayed
 */
void printList( linkedList_t *list ) 
{
	// Declare vars for iterating.
	node_t *current = list->head;
	unsigned int node = 0;
	
	printf(
			"Node\t\tType\t\tName\n"
			"======================================================\n"
	);
	// Iterate and remove nodes.
	while( current != NULL ) 
	{
		if(current != list->head && current!=list->tail) 
		{
			printf("%d\t\t%c\t\t%s\n",node,current->type,current->name);
			node++;
		}
		current=current->next;
	}
	
	return;
}

/**
 * Prints a node for debugging.
 *
 * @param list a node to be displayed
 */
void printNode( node_t *node )
{
	if(node != NULL)
	{
		printf(
				"Type\t\tName\n"
				"=========================================\n"
				"%c\t\t%s\n",
				node->type,
				node->name
		);
	}
	else
	{
		printf(
				"NULL node!\n"
		);
	}
}