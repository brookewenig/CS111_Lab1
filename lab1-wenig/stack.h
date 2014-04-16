#include <stdlib.h>
#include <stdio.h>
//#include <string.h>
#ifndef CMI
#define CMI
	#include "command-internals.h"
#endif

typedef struct command command;
//typedef enum { false = 0, true = 1 } bool;

typedef struct {
    unsigned maxSize;
    char *top;
    char *stackData; 
} CharStack;

typedef struct {
    unsigned maxSize;
    command *top;
    command *stackData; 
} CommandStack;

CharStack *newCharStack(unsigned stackLen);
void deleteStack(CharStack *);
int push(CharStack *, char);
int pop(CharStack *, char *);
int peek(CharStack *, char *);
unsigned stackLen(CharStack *);
unsigned stackMaxSize(CharStack *);

CommandStack *newCommandStack(unsigned stackLen);
void cmdDeleteStack(CommandStack *);
int cmdPush(CommandStack *, command );
int cmdPop(CommandStack *, command *);
int cmdPeek(CommandStack *, command *);
unsigned cmdStackLen(CommandStack *);
unsigned cmdStackMaxSize(CommandStack *);

