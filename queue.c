#include "newqueue.h"

CommandStack *newCommandStack(unsigned stackLen) { 
    CommandStack *stack = calloc(1,sizeof(CommandStack)); 
    stack->stackData = calloc(stackLen+1,sizeof(*(stack->stackData))); 
    stack->top = stack->stackData; 
    stack->maxSize = stackLen; 
    return stack; 
} 

unsigned cmdStackLen(CommandStack *stack) { 
    return(stack->top - stack->stackData); 
} 

unsigned cmdStackMaxSize(CommandStack *stack) { 
    return(stack->maxSize); 
} 

void cmdDeleteStack(CommandStack *stack) { 
    free(stack->stackData); 
    free(stack); 
} 

int cmdPush(CommandStack *stack, command c) { 
    if (cmdStackLen(stack) < cmdStackMaxSize(stack)) { 
        *(++stack->top) = c;
        return 1; 
    } else { 
        return 0; 
    } 
} 

int cmdPop(CommandStack *stack, command *c) { 
    if (stack->top != stack->stackData) { 
	  *c = *(stack->top--); 
	  //*c = stack->top;
	  //stack->top--;
        return 1; 
    } else { 
        return 0; 
    } 
} 

int cmdPeek(CommandStack *stack, command *c) {
    if (stack->top != stack->stackData) {
        *c = *(stack->top);
        return 1;
    } else {
        return 0;
    }
}
