// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "stack.h"
#ifndef CMI
#define CMI
	#include "command-internals.h"
#endif
#include <error.h>
#include "impread-command.h"

struct commandNode{
	struct command* command; //stuct command* defined in command-internals.h 
	struct commandNode *next;
};

typedef struct CharStack OperatorStack;


typedef struct command_stream{
	struct commandNode *head;
	struct commandNode *tail;
} command_stream;

/*
	Function declarations
*/
char* getWord(int(*get_next_byte) (void *), void *get_next_byte_argument, char *endChar);
int prec(char op);
void evaluateOperator(OperatorStack *opStack, CommandStack *cmStack, char newOperator);
command combineCommand(const command *firstCommand, const command *secondCommand, char newOperator);
void checkWordForRealloc(const int *charCount, int *cPerWord, char* word);
void checkMultWordsForRealloc(const int *wordCount, const int *charCount, int *numWords, int *cPerWord, char** word);
void clearBuffer(int *charCount, int *wordCount, int *numWords, int *cPerWord, char** buffer);

command_stream_t make_command_stream (int (*get_next_byte) (void *), void *get_next_byte_argument){
  
  command_stream_t cStream;

  //currNode will be our current node, prev will be our previous node
  //commandNode *currNode = checked_malloc(sizeof(commandNode));
  //commandNode *prevNode = checked_malloc(sizeof(commandNode));

  /*
	Here we init currNode's char** to be 4 words long,
	each word consisting of 4 characters initially
   */
  int numWords = 8, cPerWord = 8;
  int wordCount = 0, charCount = 0;
  int inSize = 8, outSize = 8;
  int inpCount = 0, outCount = 0;

  int currChar = get_next_byte(get_next_byte_argument);
  int prevChar = NULL;
  
  char** buffer = checked_malloc(sizeof(char*));

  buffer = checked_malloc(sizeof(char*) * numWords);
  int i;
  for(i = 0; i < numWords; i++){
    buffer[i] = checked_malloc(sizeof(char) * cPerWord);
  }
  
  CommandStack *cmStack = newCommandStack(100);
  OperatorStack *opStack = newCharStack(100);

  bool readNextByte;
  int nextChar;
  //BROOKE: End case for when the loop is done, creating nodes (each of which holds a complete command), printing
  while (currChar != '\0'){
	  readNextByte = false;
	  if (isNormal(currChar)){
		  if (prevChar == ' '){
			  wordCount++;
			  charCount = 0;
		  }
		  checkMultWordsForRealloc(wordCount, charCount, numWords, cPerWord, buffer);
		  buffer[wordCount][charCount] = currChar;
		  charCount++;
	  }
	  else if (isOperator(currChar)){
		  command c;
		  c.input = NULL;
		  c.output = NULL;
		  c.u.word = buffer;
		  c.type = SIMPLE_COMMAND;
		  cmdPush(cmStack, c);

		  clearBuffer(charCount, wordCount, numWords, cPerWord, buffer);

		  char newOperator;
		  switch (currChar){
		  case '|':
			  nextChar = get_next_byte(get_next_byte_argument);
			  if (nextChar == '|')
				  newOperator = 'o';
			  else{
				  readNextByte = true;
				  newOperator = '|';
			  }
			  break;
		  case '&':
			  nextChar = get_next_byte(get_next_byte_argument);
			  if (nextChar == '&')
				  newOperator = 'a';
			  else{
				  readNextByte = true;
				  fprintf(stderr, "YOU DONE GIVE ME SOME BAD INPUT");
			  }
			  break;
		  case '(':
			  newOperator = '(';
			  break;
		  case ')':
			  newOperator = ')';
			  break;
		  }

		  evaluateOperator(opStack, cmStack, newOperator);
	  }
	  else if (isRedirect(currChar)){
		  command c;
		  c.input = NULL;
		  c.output = NULL;
		  c.u.word = buffer;
		  c.type = SIMPLE_COMMAND;
		  cmdPush(cmStack, c);

		  clearBuffer(charCount, wordCount, numWords, cPerWord, buffer);

		  char *word = getWord(get_next_byte, get_next_byte_argument, nextChar);
		  if (word != NULL){
			  command topCommand;
			  pop(cmStack, &topCommand);
			  if (currChar == '<')
				  topCommand.input = word;
			  else
				  topCommand.output = word;
			  cmdPush(cmStack, topCommand);

			  readNextByte = true;
		  }
		  else{
			  //BADLY FORMATTED
		  }
	  }

	  if (readNextByte){
		  currChar = nextChar;
		  prevChar = currChar;
	  }
	  else{
		  prevChar = currChar;
		  currChar = get_next_byte(get_next_byte_argument);
	  }
  }
  return 0;
}

char* getWord(int (*get_next_byte) (void *), void *get_next_byte_argument, char *endChar){
  int currChar = get_next_byte(get_next_byte_argument);
  int prevChar = NULL;
  int charCount = 0, wordSize = 8;
  char* miniBuffer = checked_malloc(sizeof(char) * wordSize);
  while(currChar != '\0'){
	if(!isNormal(currChar) && isNormal(prevChar)){
		*endChar = currChar;
		return miniBuffer;
	} 
	else if(isNormal(currChar)){
		checkWordForRealloc(charCount, wordSize, miniBuffer);
		miniBuffer[charCount++] = currChar;
	}
	prevChar = currChar;
	currChar = get_next_byte(get_next_byte_argument);
  }
  if (isNormal(prevChar)){
	  return miniBuffer;
  }
  return NULL;
}

void clearBuffer(int *charCount, int *wordCount, int *numWords, int *cPerWord, char** buffer){
	/*for (int i = 0; i < *numWords; i++){
		free(buffer[i]);
	}
	free(buffer);*/

	charCount = 0;
	wordCount = 0;
	numWords = 8;
	cPerWord = 8;
	buffer = checked_malloc(sizeof(char*) * (*numWords));
	int i;
	for (i = 0; i < *numWords; i++){
		buffer[i] = checked_malloc(sizeof(char) * (*cPerWord));
	}
}

int prec(char op){
  switch(op){
  case '(':
	return -1;
  case '\n':
  case ';':
	return 0;
  case 'a':
  case 'o':
	return 1;
  case '|':
	return 2;
  }
}

//NOTE: functions such as pop() or push() work on CharStack, cmdPop() or cmdPush() work on CommandStack
void evaluateOperator(OperatorStack *opStack, CommandStack *cmStack, char newOperator){
  if(stackLen(opStack) == 0 || newOperator == '('){
	push(opStack, newOperator);
  }
  else if(newOperator == ')'){
	char topOperator;
	peek(opStack, &topOperator);
	while(topOperator != '('){
	  char currOp;
	  command secondCommand, firstCommand;
	  pop(opStack, &currOp);
	  cmdPop(cmStack, &secondCommand);
	  cmdPop(cmStack, &firstCommand);
	  command newCommand = combineCommand(&firstCommand,&secondCommand,currOp);
	  cmdPush(cmStack, newCommand);
	  if(stackLen(opStack)>0)
		peek(opStack, topOperator);
	  else
		break;
	}
	pop(opStack, topOperator);
	command *subShellCommand = checked_malloc(sizeof(command));
	subShellCommand->status = -1;
	subShellCommand->type = SUBSHELL_COMMAND;
	subShellCommand->input = NULL;
	subShellCommand->output = NULL;
	cmdPop(cmStack, subShellCommand->u.subshell_command);
	cmdPush(cmStack, *subShellCommand);
  }
  else{
	char topOperator;
	peek(opStack, &topOperator);
	if(prec(newOperator) > prec(topOperator)){
	  push(opStack, newOperator);
	}
	else{
	  while(topOperator != '(' && prec(newOperator) <= prec(topOperator)){
		char currOp;
		command secondCommand, firstCommand;
		pop(opStack, &currOp);
		cmdPop(cmStack, &secondCommand);
		cmdPop(cmStack, &firstCommand);
		command newCommand = combineCommand(&firstCommand,&secondCommand,currOp);
		cmdPush(cmStack, newCommand);
		if(stackLen(opStack)>0)
		  peek(opStack, topOperator);
		else
		  break;
	  }
	  push(opStack, newOperator);
	}
  }
}

command combineCommand(const command *firstCommand, const command *secondCommand, char newOperator){
  command newCommand;
  switch(newOperator){
  case '\n':
  case ';':
	newCommand.type = SEQUENCE_COMMAND;
  break;
  case 'a':
	newCommand.type = AND_COMMAND;
  case 'o':
	newCommand.type = OR_COMMAND;
  case '|':
	newCommand.type = PIPE_COMMAND;
  }
  newCommand.u.command[0] = firstCommand;
  newCommand.u.command[1] = secondCommand;
  newCommand.input = NULL;
  newCommand.output= NULL;
  return newCommand;
}

//void checkInForRealloc(int inpCount, int &inSize, commandNode *currNode){
//  if(inpCount >= inSize){
//    /*
//      If we have more characters in the input than we initially allocated for
//	  realloc the input
//	*/
//    inSize *= 2;
//	char* tmp = checked_realloc(currNode->command.input, sizeof(char) * inSize);
//	currNode->command.input = tmp;
//  }
//}
//
//void checkOutForRealloc(int outCount, int &outSize, commandNode *currNode){
//  if(outCount >= outSize){
//	/*
//      If we have more characters in the output than we initially allocated for
//      realloc the output
//	*/
//	outSize *= 2;
//	char* tmp = checked_realloc(currNode->command.output, sizeof(char) * outSize);
//	currNode->command.output = tmp;
//  }
//}

void checkWordForRealloc(const int *charCount, int *cPerWord, char* word){
	if (*charCount >= *cPerWord){
		//If we have more words than we initially allocated for, realloc the words
		*cPerWord *= 2;
		char* tmp = checked_realloc(word, sizeof(char) * (*cPerWord));
		word = tmp;
	}
}
void checkMultWordsForRealloc(const int *wordCount, const int *charCount, int *numWords, int *cPerWord, char** word){
  if(*wordCount >= *numWords){
	  //If we have more words than we initially allocated for, realloc the words
	  *numWords *= 2;
	  char** tmp = checked_realloc(word, sizeof(char*) * (*numWords));
	  word = tmp;
  }
  if(*charCount >= *cPerWord){
	/*
	  If we have more characters in a word than we initially allocated for,
	  go through and realloc each word to be the new size
	*/
	*cPerWord *= 2;
	int i;
	for(i = 0; i < *numWords; i++){
	  char* tmp = checked_realloc(word[i], sizeof(char) * (*cPerWord));
		word[i] = tmp;
	}
  }
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  //error (1, 0, "command reading not yet implemented");
  return 0;
}
