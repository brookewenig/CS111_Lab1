#include "commandstack.h"

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

bool cmdPush(CommandStack *stack, command c) { 
    if (cmdStackLen(stack) < cmdStackMaxSize(stack)) { 
        (*(++stack->top)) = c; 
        return true; 
    } else { 
        return false; 
    } 
} 

bool cmdPop(CommandStack *stack, command *c) { 
    if (stack->top != stack->stackData) { 
        *c = *(stack->top--); 
        return true; 
    } else { 
        return false; 
    } 
} 

bool cmdPeek(CommandStack *stack, command *c) {
    if (stack->top != stack->stackData) {
        *c = *(stack->top);
        return true;
    } else {
        return false;
    }
}
