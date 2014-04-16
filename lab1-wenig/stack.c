#include "stack.h"

CharStack *newCharStack(unsigned stackLen) { 
    CharStack *stack = calloc(1,sizeof(CharStack)); 
    stack->stackData = calloc(stackLen+1,sizeof(*(stack->stackData))); 
    stack->top = stack->stackData; 
    stack->maxSize = stackLen; 
    return stack; 
} 

unsigned stackLen(CharStack *stack) { 
    return(stack->top - stack->stackData); 
} 

unsigned stackMaxSize(CharStack *stack) { 
    return(stack->maxSize); 
} 

void deleteStack(CharStack *stack) { 
    free(stack->stackData); 
    free(stack); 
} 

int push(CharStack *stack, char c) { 
    if (stackLen(stack) < stackMaxSize(stack)) { 
        (*(++stack->top)) = c; 
        return 1; 
    } else { 
        return 0; 
    } 
} 

int pop(CharStack *stack, char *c) { 
    if (stack->top != stack->stackData) { 
        *c = *(stack->top--); 
        return 1; 
    } else { 
        return 0; 
    } 
} 

int peek(CharStack *stack, char *c) {
    if (stack->top != stack->stackData) {
        *c = *(stack->top);
        return 1;
    } else {
        return 0;
    }
}

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
