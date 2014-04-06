#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "command-internals.h"

typedef enum { false = 0, true = 1 } bool;
typedef struct command command;

typedef struct {
    unsigned maxSize;
    command *top;
    command *stackData; 
} CommandStack;

CommandStack *newCommandStack(unsigned stackLen);
void cmdDeleteStack(CommandStack *);
bool cmdPush(CommandStack *, command);
bool cmdPop(CommandStack *, command *);
bool cmdPeek(CommandStack *, command *);
unsigned cmdStackLen(CommandStack *);
unsigned cmdStackMaxSize(CommandStack *);

