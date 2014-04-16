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
#include "SymbolList.h"


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
	
	deleteList(undefined);
	deleteList(defined);
}

static void handleObjectFile(char * filename, linkedList_t * defined, linkedList_t * undefined)
{
	char command[120]="nm ";
	strcat(command, filename);
	char buffer[120];
	FILE *fp;
	fp = popen(command, "r");
	
	// you need to write this displayMessageAndExit function
	if (fp == NULL)  displayMessageAndExit("popen failed\n");
	
	char symbolType = '\0';
	char symbolName[31];
	while (fgets(buffer, sizeof(buffer), fp))
	{
		sscanf(buffer,"%*d %c %s",&symbolType,symbolName);
		handleObjectSymbol(symbolType,&symbolName,defined,undefined);
	}
	pclose(fp);

	return;
    // printf("object file: %s\n", filename);
}

handleObjectSymbol(char symbolType, char * symbolName, linkedList_t * defined, linkedList_t * undefined) 
{
	static unsigned int suffix; // STOPPED WORKING HERE!
	switch(symbolType)
	{
		case 'U':
			if(searchList(defined,symbolName)==NULL && searchList(undefined,symbolName)==NULL)
				insertNode(undefined,symbolName,symbolType);
			break;
		case 'C':
			if(searchList(defined,symbolName)==NULL)
			{
				insertNode(defined,symbolName,symbolType);
			}
			removeNode(undefined,symbolName);
			break;
		case 'A':
			
			break;
	}
}

static void displayMessageAndExit(char * message)
{
	printf("%s",message);
	exit(EXIT_FAILURE);
}

static void handleArchive(char * filename, linkedList_t * defined, linkedList_t * undefined)
{
	char *fileName = strcat("nm ", filename);
	char buffer[120];
	FILE *fp;
	fp = popen(fileName, "r");
	
	//you need to write this displayMessageAndExit function
	if (fp == NULL)  displayMessageAndExit("popen failed\n");
	
	char symbolType = '\0';
	char symbolName[31];
	while (fgets(buffer, sizeof(buffer), fp))
	{
		sscanf("%*d\t%c\t%s",&symbolType,symbolName);
		printf(
				"Symbol:\t%c\n"
				"Name:\t%s\n\n",
				symbolType,
				symbolName
		);
	}
	pclose(fp);

	return;
    // printf("archive file: %s\n", filename);
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
