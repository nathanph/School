/* 
 * Name: <Nathan Hernandez>
 * Description: This program performs the resolution task of a linker.
 *              It takes as input a list of .o files and archive files
 *              and keeps track of undefined and defined symbols.  All
 *              symbols in a .o file are added to one of the two lists.
 *              Archived object files are handled a little differently.
 *              If there is a symbol that is currently undefined, but defined
 *              in a object file within an archive, then all symbols in that
 *              archived object file are added to one of the two lists; 
 *              otherwise, no symbols from that archive member are added.  
 *              Members of an archive are visited repeatedly until there are 
 *              no changes in the lists of defined and undefined symbols.
 *
 *              If at some point there is an attempt to add a second strong
 *              symbol to the defined symbols list, then an error message is
 *              displayed indicating the multiple definition.
 *
 *              At the end of the process, the program looks to make sure that
 *              the main function is defined and if not, an error message is
 *              displayed.  Next, the program prints out the entries of the 
 *              undefined symbols list with an error message for each one.
 */

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "resolve.h"

int main(int argc, char * argv[])
{
    int i, istat; 
    struct stat stFileInfo;
	
	linkedList_t *defined = initializeList();
	linkedList_t *undefined = initializeList();
	
    if (argc <= 1)
    {
       printf("resolve: no input files\n");
       exit(1);
    }
    for (i = 1; i < argc; i++)
    {
        istat = stat(argv[i], &stFileInfo);
        //if istat is 0 then file exists
        if (istat == 0)
        {
            if (!isObjectFile(argv[i]) && !isArchive(argv[i]))
            {
                printf("%s: file not recognized\n", argv[i]);
            } else {
                if (isArchive(argv[i])) handleArchive(argv[i], defined, undefined);
                if (isObjectFile(argv[i])) handleObjectFile(argv[i], defined, undefined);
            }
        } else {
            printf("%s: file not found\n", argv[i]);
        }
    }
	
	if( searchList(defined,"main") == NULL )
	{
		printf(": undefined reference to main\n");
	}
	
	node_t *node = undefined->head;
	
	while( node != NULL ) {
		if(node != undefined->head && node!=undefined->tail)
		{
			printf(": undefined reference to %s\n", node->name);
		}
		node=node->next;
	}
	 
	printf("Defined Symbol Table\n");
	printList(defined);
	// printf("\nUndefined Symbol Table:\n");
	// printList(undefined);
	
	deleteList(undefined);
	deleteList(defined);
	return 0;
}

static void handleObjectFile(char * filename, linkedList_t * defined, linkedList_t * undefined)
{
	char command[120];
	char buffer[120];
	FILE *fp;
	
	sprintf(command, "nm %s", filename);
	// printf("%s\n",command);	
	fp = popen(command, "r");
	if (fp == NULL)  displayMessageAndExit("popen failed\n");
	
	char symbolType = '\0';
	char symbolName[31];
	while (fgets(buffer, sizeof(buffer), fp))
	{
		// puts(buffer);
		sscanf(buffer,"%*9c %c %s",&symbolType,symbolName);         
		handleObjectSymbol(symbolType,&symbolName[0],defined,undefined);
	}
	pclose(fp);

	return; 
}

static bool handleArchiveObjectFile(char * filename, linkedList_t * defined, linkedList_t * undefined)
{
	
	char command[120];
	char buffer[120];
	FILE *fp;
		
	sprintf(command,"nm %s",filename);
	// printf("%s\n",command);	
	fp = popen(command, "r");
	if (fp == NULL)  displayMessageAndExit("popen failed\n");
	
	char symbolType = '\0';
	char symbolName[31];
	node_t *node = NULL;
	
	while (fgets(buffer, sizeof(buffer), fp))
	{
		// puts(buffer);
		sscanf(buffer,"%*9c %c %s",&symbolType,symbolName);         

		if( (symbolType=='C' || symbolType=='T' || symbolType=='D') && 
				searchList(undefined,&symbolName[0])!=NULL)
		{
			return true;
		}
		else if( (symbolType=='D' || symbolType=='T') && 
				(node=searchList(defined,&symbolName[0]))!=NULL && 
				node->type=='C' )
		{
			return true;
		}
	}
	pclose(fp);

	return false; 
}

static void handleArchive(char * filename, linkedList_t * defined, linkedList_t * undefined)
{
	char command[120];
	
	sprintf(command, "rm -f -r tmp");
	// printf("%s\n",command);	
	system(command);
	
	sprintf(command, "mkdir tmp");
	// printf("%s\n",command);	
	system(command);
	
	sprintf(command,"cp %s ./tmp/",filename);
	// printf("%s\n",command);	
	system(command);
	
	sprintf(command,"cd tmp; ar -x %s",filename);
	// printf("%s\n",command);	
	system(command);
	
	FILE *fp;
	bool changed = false;
	do {
		sprintf(command, "ls ./tmp/*.o");
		
		fp = popen(command, "r");
		if (fp == NULL)  displayMessageAndExit("popen failed\n");
		
		char objectFile[80];
		changed=false;
		while (fgets(objectFile, sizeof(objectFile), fp))
		{
			sscanf("%s",objectFile);
			if(handleArchiveObjectFile(&objectFile[0],defined,undefined))
			{
				handleObjectFile(&objectFile[0], defined, undefined);
				changed=true;
			}
		}
		pclose(fp);
	} while (changed==true);
	return;
	
	sprintf(command, "rm -f -r tmp");
	// printf("%s\n",command);
	system(command);
}

static void handleObjectSymbol(char symbolType, char * symbolName, linkedList_t * defined, 
		linkedList_t * undefined) 
{
	node_t *node = NULL;
	static unsigned int suffix=0;
	switch(symbolType)
	{
		case 'U':
			if(searchList(defined,symbolName)==NULL && 
					searchList(undefined,symbolName)==NULL)
			{
				insertNode(undefined,symbolName,symbolType);
			}
			break;
		case 'C':
			if(searchList(defined,symbolName)==NULL)
			{
				insertNode(defined,symbolName,symbolType);
			}
			removeNode(undefined,symbolName);
			break;
		case 'b':
		case 'd':
			sprintf( symbolName, "%s.%u", symbolName, suffix++);
			insertNode(defined,symbolName,symbolType);
			break;
		case 'T':
		case 'D':
			node=searchList(defined,symbolName);
			if(node!=NULL)
			{
				switch(node->type)
				{
					case 'T':
					case 'D':
						printf(": multiple definition of %s\n",symbolName);
						break;
					case 'C':
						updateNode(defined,symbolName,symbolType);
						break;
				}
			}
			else
			{
				insertNode(defined,symbolName,symbolType);
			}
			node=searchList(undefined,symbolName);
			if(node!=NULL)
			{
				removeNode(undefined,symbolName);
			}
			break;
	}
}

static void displayMessageAndExit(char * message)
{
	printf("%s",message);
	exit(EXIT_FAILURE);
}

/* 
 * function: isArchive
 * description: This function takes as input a c-string and returns
 *              true if the c-string ends with a .a extension.
 * input: filename 
 * returns: 1 or 0
 */
static bool isArchive(char * filename)
{
    int len = strlen(filename);
    if (len < 3) 
        return false;
    if (filename[len - 2] != '.')
        return false;
    if (filename[len-1] != 'a')
        return false;
    return true;
}

/* 
 * function: isObjectFile
 * description: This function takes as input a c-string and returns
 *              true if the c-string ends with a .o extension.
 * input: filename 
 * returns: 1 or 0
 */
static bool isObjectFile(char * filename)
{
    int len = strlen(filename);
    if (len < 3) 
        return false;
    if (filename[len - 2] != '.')
        return false;
    if (filename[len-1] != 'o')
        return false;
    return true;
}
