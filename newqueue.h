#include <stdlib.h>
#include <stdio.h>
//#include <string.h>
#ifndef CMI
#define CMI
	#include "command-internals.h"
#endif

typedef struct command command;
//typedef enum { false = 0, true = 1 } bool;

/*typedef struct {
    unsigned maxSize;
    command *top;
    command *stackData; 
} CommandStack;


CommandStack *newCommandStack(unsigned stackLen);
void cmdDeleteStack(CommandStack *);
int cmdPush(CommandStack *, command );
int cmdPop(CommandStack *, command *);
int cmdPeek(CommandStack *, command *);
unsigned cmdStackLen(CommandStack *);
unsigned cmdStackMaxSize(CommandStack *);
*/
