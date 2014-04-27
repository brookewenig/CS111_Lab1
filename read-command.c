// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "stack.h"
#ifndef CMI
#define CMI
	#include "command-internals.h"
#endif
#include <error.h>
#include "impread-command.h"

typedef struct commandNode{
	struct command* command; //stuct command* defined in command-internals.h 
	struct commandNode *next;
} commandNode;

typedef struct command_stream{
  struct commandNode *head;
  struct commandNode *tail;
  int size;
} command_stream;

typedef enum { EMPTY, NONEMPTY} bufferStatus;
bufferStatus buffStatus = EMPTY;
//typedef enum {EMPTY, NONEMPTY} commandStatus;
bufferStatus cmdStatus = EMPTY;
/*
	Function declarations
*/
char* getWord(int(*get_next_byte) (void *), void *get_next_byte_argument, char *endChar, char *secondToLast);
int prec(char op);
void evaluateOperator(CharStack *opStack, CommandStack *cmStack, char newOperator);
command* combineCommand(command *firstCommand, command *secondCommand, char newOperator);
char* checkWordForRealloc(const int *charCount, int *cPerWord, char* word);
char** checkMultWordsForRealloc(const int *wordCount, const int *charCount, int *numWords, int *cPerWord, char** word);
char** clearBuffer(int *charCount, int *wordCount, int *numWords, int *cPerWord, char** buffer);

char ** buffer; 
int numLines = 1; 
//command *firstCommand, *secondCommand;

//Do the malloced contents of buffer only survive in the function make_command_stream???
command_stream_t make_command_stream (int (*get_next_byte) (void *), void *get_next_byte_argument){
  //firstCommand = (command*)malloc(sizeof(command));
  //secondCommand = (command*)malloc(sizeof(command));
  command_stream_t cStream = (command_stream_t)malloc(sizeof(command_stream));
  cStream->size = 0;
  //currNode will be our current node, prev will be our previous node
  //commandNode *currNode = malloc(sizeof(commandNode));
  //commandNode *prevNode = malloc(sizeof(commandNode));

  /*
	Here we init currNode's char** to be 4 words long,
	each word consisting of 4 characters initially
   */
  int numWords = 4, cPerWord = 8;
  int wordCount = 0, charCount = 0;
 
  char currChar = (char)get_next_byte(get_next_byte_argument);
  char prevChar = '~';
  //buffer = (char**)malloc(sizeof(char*));

  buffer = (char**)malloc(sizeof(char*) * numWords);
  int i;
  for(i = 0; i < numWords; i++){
    buffer[i] = (char*)malloc(sizeof(char) * cPerWord);
  } //Moved to outside of function
  
  CommandStack *cmStack = newCommandStack(100);
  CharStack *opStack = (CharStack*)newCharStack(100);
  bool readNextByte;
  char nextChar;
  char dblLookBack;
  bool recentNewLine = false;
  //BROOKE: End case for when the loop is done, creating nodes (each of which holds a complete command), printing
  while (currChar != -1) {// && currChar !='~'){ //added the '\n' because our test file is ended with a newline
//then null (-1) as opposed to nullbyte
	if (currChar == '\n')
		numLines++;  
	readNextByte = false;
	  //ADD CHECK ING FOR BEING IN MIDDLE OF A COMMAND
	/*  if (validComment(currChar, prevChar)) {
		while (currChar != '\n') {
			prevChar = currChar;
			currChar = get_next_byte(get_next_byte_argument);//keep reading until get newline
	  	}
	  }*/
	  if (isRedirect(currChar) && isRedirect(prevChar)) 
		  error(1,0,"%d: too many operators", numLines);
//	if (isOperator(currChar) && stackLen(opStack) == 0)
//		error(1,0, "need command before operator", "Cool Program"); 
	//if (isOperator(currChar) && prevChar == '`')
	//	error(1,0, "Invalid", "Cool Program");
//	if (!isNormal(prevChar) && !isSpecial(prevChar) && prevChar != '\n' && prevChar != ':' && prevChar != '~')
//		error(1,0, "%d: Bad prevChar", numLines);
	  //int notGood = (currChar != ' ');
	if(!isNormal(currChar) && !isSpecial(currChar) && currChar != ' ' && currChar != '\n' && currChar !='#') {
		error(1,0, "%d: Invalid token encountered fool!", numLines);
	}
	if(currChar == '\n' && prevChar != '\n'){
	  recentNewLine = true;
	  dblLookBack = prevChar;
	}
	//	if(prevChar == '\n' && currChar != '\n' && currChar != '(' && currChar != ')' && !isNormal(currChar)){
	// error(1,0, "%d: Newlines may only be followed by (, ), or a normal token", numLines);
	//}
	  if (validComment(currChar, prevChar)) {
        while (currChar != '\n') {
		  prevChar = currChar;
		  currChar = get_next_byte(get_next_byte_argument);//keep reading until get newline
        }
		//readNextByte = true;
      }
	  else if(currChar == '\n' && prevChar == '\n' && !isSpecial(dblLookBack)){
		if(buffStatus == NONEMPTY){
		  command *c = (command*)malloc(sizeof(command));;
		  c->input = NULL;
		  c->output = NULL;
		  c->u.word = buffer;
		  c->status = -1;
		  c->type = SIMPLE_COMMAND;
		  cmdPush(cmStack, *c);
		
		  //BUFFERFIX
		  buffer = clearBuffer(&charCount, &wordCount, &numWords, &cPerWord, buffer);
		}

		while(stackLen(opStack)){
		  char currOp;
		  command *firstCommand, *secondCommand;
		  firstCommand = (command*)malloc(sizeof(command));
		  secondCommand = (command*)malloc(sizeof(command));

		  pop(opStack, &currOp);
		  cmdPop(cmStack, secondCommand);
		  cmdPop(cmStack, firstCommand);
		  command *com = combineCommand(firstCommand,secondCommand,currOp);
		  cmdPush(cmStack, *com);
		}
		commandNode *next = (commandNode*)malloc(sizeof(commandNode));
		next->command = (command*)malloc(sizeof(command));
		cmdPop(cmStack, next->command);
		next->next = NULL;
		if(cStream->size > 1){
		  cStream->tail->next = next;
		  cStream->tail = next;
		}
		else if(cStream->size == 1){
		  cStream->head->next = next;
		  cStream->tail = next;
		}
		else{
		  cStream->tail = next;
		  cStream->head = next;
		}
		cStream->size++;
	  }
	  else if(currChar == '\n' && prevChar == '\n' && isSpecial(dblLookBack)){
		  if(isRedirect(dblLookBack)){
		//	fprintf(stderr, "%d: Bad Stuff", numLines);
			error(1,0,"%d: Bad stuff", numLines);
		  }
		  else{
			//int foo = 0;
		  }
	  }
	  else if (isNormal(currChar)){
		recentNewLine = false;
//		buffStatus = NONEMPTY;
		  /*if (prevChar == ' ' && wordCount ==0)
		{
		  checkMultWordsForRealloc(&wordCount, &charCount, &numWords, &cPerWord, buffer);
		  buffer[wordCount][charCount] = currChar;
		  wordCount++;
		  charCount++;
		}*/
		buffStatus = NONEMPTY;
		if (prevChar == ' ' && cmdStatus == EMPTY)
		{
		  buffer = checkMultWordsForRealloc(&wordCount, &charCount, &numWords, &cPerWord, buffer);
		  buffer[wordCount][charCount] = currChar;
		  charCount = 1;
		  cmdStatus = NONEMPTY;	
		}
		 else if (prevChar == ' '){
		  wordCount++;
		  charCount = 0;
		  buffer = checkMultWordsForRealloc(&wordCount, &charCount, &numWords, &cPerWord, buffer);
		  buffer[wordCount][charCount] = currChar;
		  charCount++;
		  cmdStatus = NONEMPTY;
		  }

		else {
		 buffer = checkMultWordsForRealloc(&wordCount, &charCount, &numWords, &cPerWord, buffer);
		 buffer[wordCount][charCount] = currChar;
		 charCount++;
		cmdStatus = NONEMPTY;
		}
	  }
	  else if (isOperator(currChar)){
		if(recentNewLine && currChar != '(' && currChar != ')')
		  error(1,0,"%d: Newline in a non valid location", numLines); 
		if(buffStatus == NONEMPTY){
		  command *c = (command*)malloc(sizeof(command));
		  c->input = NULL;
		  c->status = -1;
		  c->output = NULL;
		  c->u.word = buffer;
		  cmdStatus = EMPTY;
		  c->type = SIMPLE_COMMAND;
		  cmdPush(cmStack, *c);	
		  buffer = clearBuffer(&charCount, &wordCount, &numWords, &cPerWord, buffer);
		}
		
		  char newOperator;
		  switch (currChar){
		  case '|':
			nextChar = (char)get_next_byte(get_next_byte_argument);
			  if (nextChar == '|')
				  newOperator = 'o';
			  else{
				  readNextByte = true;
				  newOperator = '|';
			  }
			  break;
		  case '&':
			nextChar = (char)get_next_byte(get_next_byte_argument);
			  if (nextChar == '&')
				  newOperator = 'a';
			  else{
				  readNextByte = true;
				  error(1,0, "%d: Bad input", numLines);
				  //fprintf(stderr, "YOU DONE GIVE ME SOME BAD INPUT");
			  }
			  break;
		  case '(':
			  newOperator = '(';
			  break;
		  case ')':
			  newOperator = ')';
			  break;
		  case ';':
			newOperator = ';';
			break;
		  }

		  evaluateOperator(opStack, cmStack, newOperator);
	  }
	  else if (isRedirect(currChar)){
		command *c = (command*)malloc(sizeof(command));
		if(buffStatus == NONEMPTY){
		  c->input = NULL;
		  c->output = NULL;
		  c->u.word = buffer;
		  c->type = SIMPLE_COMMAND;
		  cmdPush(cmStack, *c);
		  cmdStatus = EMPTY;
		//BUFFERFIX
		  buffer = clearBuffer(&charCount, &wordCount, &numWords, &cPerWord, buffer);
		}
		else if(cmdStackLen(cmStack)==0)
			error(1,0, "%d: Redirect must have command before it!", numLines);
 
		// error(1,0,"Redirect must have command before it!", "Cool Program");
		char secondToLast;
		char *word = getWord(get_next_byte, get_next_byte_argument, &nextChar, &secondToLast);
		  if (word != NULL){
			// command *topCommand;
			  cmdPop(cmStack, c);
			  if (currChar == '<')
				  c->input = word;
			  else
				  c->output = word;
			  cmdPush(cmStack, *c);

			  readNextByte = true;

			  currChar = secondToLast;
		  }
		  else{
			error(1,0,"%d: Redirect badly formatted", numLines);

		//	error(1,0,"Redirect badly formatted!", "Cool Program");
		  }
	  }

	  if (readNextByte){
		prevChar = currChar;
		  currChar = nextChar;
	  }
	  else{
		  prevChar = currChar;
		  currChar = get_next_byte(get_next_byte_argument);
	  }
  }
  if(buffStatus == NONEMPTY){//stackLen(opStack) == 0 && buffStatus == NONEMPTY){
	command *c = (command*)malloc(sizeof(command));
	c->input = NULL;
	c->status = -1;
	c->output = NULL;
	//int i;	
	//c->u.word = (char**)calloc(1, sizeof(char*) * (numWords));
	//int i;
	/* for (i = 0; i < wordCount; i++){ */
	/*   c->u.word[i] = (char*)calloc(1, sizeof(char) * (cPerWord)); */
	/* } */
	/* int j; */
	/* for (i = 0;i < wordCount; i++) { */
	/* 	for (j=0; j < cPerWord; j++) { */
	/* 		c->u.word[i][j] = buffer[i][j]; */
	/* 	} */
	/* } */

	//BUFFERFIX
	c->u.word = buffer;
	c->type = SIMPLE_COMMAND;
  	cmdPush(cmStack, *c);
  }
  while(stackLen(opStack)){
	if(cmdStackLen(cmStack)<2)
		error(1,0, "%d: Mismatch in number of operators and operands!", numLines);
	// error(1,0,"Mismatch in number of operators and operands!", "Cool Program");
	char currOp;
	command *firstCommand, *secondCommand;
	firstCommand = (command*)malloc(sizeof(command));
	secondCommand = (command*)malloc(sizeof(command));
	pop(opStack, &currOp);
	cmdPop(cmStack, secondCommand);
	cmdPop(cmStack, firstCommand);
	command *c = combineCommand(firstCommand,secondCommand,currOp);
	cmdPush(cmStack, *c);
  }
  commandNode *tail; //Brooke: just changed from commandNode to commandNode*
//TODO: need assign type to tail.command?
  tail = (commandNode*)malloc(sizeof(commandNode));	
  tail->command = (command*)malloc(sizeof(command));
  cmdPop(cmStack, tail->command);
  tail->next = NULL;
  if(cStream->size > 1){
    cStream->tail->next = tail;
	cStream->tail = tail;
  }
  else if(cStream->size == 1){
    cStream->head->next = tail;
    cStream->tail = tail;
  }
  else{
	cStream->tail = tail;
	cStream->head = tail;
  }
  cStream->size++;
  return cStream;
}

char* getWord(int (*get_next_byte) (void *), void *get_next_byte_argument, char *endChar, char *secondToLast){
  char currChar = (char)get_next_byte(get_next_byte_argument);
  char prevChar = 0;
  int charCount = 0, wordSize = 8;
//BUFFERFIX: need update miniBuffer?
  char *miniBuffer = (char*)malloc(sizeof(char) * wordSize);
  while(currChar != -1){
	if(!isNormal(currChar) && isNormal(prevChar)){
	  *secondToLast = prevChar;
		*endChar = currChar;
		return miniBuffer;
	} 
	else if(isNormal(currChar)){
		miniBuffer = checkWordForRealloc(&charCount, &wordSize, miniBuffer);
		miniBuffer[charCount++] = currChar;
	} 
	prevChar = currChar;
	currChar = get_next_byte(get_next_byte_argument);
	if(!isNormal(currChar) && !isNormal(prevChar)){
		error(1,0, "%d: Bad stuff following redirect", numLines);
	// error(1,0, "Bad stuff following redirect", "Cool Program");
	}
  }
  if (isNormal(prevChar)){
	*endChar = currChar;
	*secondToLast = prevChar;
	  return miniBuffer;
  }
  return NULL;
}

char** clearBuffer(int *charCount, int *wordCount, int *numWords, int *cPerWord, char** buffer){
	int i;
	//BUFFERFIX
	
	/* for (i = 0; i < *numWords; i++){ */
	/* 	free(buffer[i]); */
	/* } */
	/* free(buffer); */

	*charCount = 0;
	*wordCount = 0;
	*numWords = 4;
	*cPerWord = 8;
	buffer = (char**)calloc(1, sizeof(char*) * (*numWords));
	//int i;
	for (i = 0; i < *numWords; i++){
	  buffer[i] = (char*)calloc(1, sizeof(char) * (*cPerWord));
	}

	buffStatus = EMPTY;
	return buffer;
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
  default:
	return -100;
  }
}

//NOTE: functions such as pop() or push() work on CharStack, cmdPop() or cmdPush() work on CommandStack
void evaluateOperator(CharStack *opStack, CommandStack *cmStack, char newOperator){
  if(stackLen(opStack) == 0 || newOperator == '('){
	push(opStack, newOperator);
  }
  else if(newOperator == ')'){
	char topOperator;
	peek(opStack, &topOperator);
	while(topOperator != '('){
	  char currOp;
	  command *firstCommand, *secondCommand;
	  firstCommand = (command*)malloc(sizeof(command));
	  secondCommand = (command*)malloc(sizeof(command));
	  pop(opStack, &currOp);
	  cmdPop(cmStack, secondCommand);
	  cmdPop(cmStack, firstCommand);
	  command *c = combineCommand(firstCommand,secondCommand,currOp);
	  cmdPush(cmStack, *c);
	  if(stackLen(opStack)>0)
		peek(opStack, &topOperator);
	  else
		error(1,0, "%d: Paren mismatch!", numLines);
	//	error(1,0,"Paren mismatch!", "Cool Program");
	}
	pop(opStack, &topOperator);
	command *subShellCommand = (command*)malloc(sizeof(command));
	subShellCommand->u.subshell_command = (command*)malloc(sizeof(command));
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
		if(cmdStackLen(cmStack)<2)
			error(1,0,"%d: Mismatch of operators and operands", numLines);
		//  error(1,0,"Mismatch of operators and commands", "Cool Program");
		command *firstCommand, *secondCommand;
		firstCommand = (command*)malloc(sizeof(command));
		secondCommand = (command*)malloc(sizeof(command));
		pop(opStack, &currOp);
		cmdPop(cmStack, secondCommand);
		cmdPop(cmStack, firstCommand);
		//TRY MKAING FIRST AND SECOND LOCAL BUT WITH MALLOC
		command* c = combineCommand(firstCommand,secondCommand,currOp);
		cmdPush(cmStack,*c);
		if(stackLen(opStack)>0)
		  peek(opStack, &topOperator);
		else
		  break;
	  }
	  push(opStack, newOperator);
	}
  }
}

//CHECK ALL STACK OPERATIONS FOR PROPER REFERENCING

command* combineCommand(command *firstCommand, command *secondCommand, char newOperator){
  command *c = (command*)malloc(sizeof(command));
  switch(newOperator){
  case '\n':
  case ';':
	c->type = SEQUENCE_COMMAND;
    break;
  case 'a':
	c->type = AND_COMMAND;
	break;
  case 'o':
	c->type = OR_COMMAND;
	break;
  case '|':
	c->type = PIPE_COMMAND;
	break;
  }
  c->u.command[0] = firstCommand;
  c->u.command[1] = secondCommand;
  c->input = NULL;
  c->status = -1;
  c->output= NULL;
  return c;
}

//I don't like the tmp* and tmp** - who is passing in word?
char* checkWordForRealloc(const int *charCount, int *cPerWord, char* word){
	if (*charCount >= *cPerWord){
		//If we have more words than we initially allocated for, realloc the words
		*cPerWord *= 2;
		char* tmp = (char*)realloc(word, sizeof(char) * (*cPerWord));
		word = tmp;
	}
	return word;
}
char** checkMultWordsForRealloc(const int *wordCount, const int *charCount, int *numWords, int *cPerWord, char** word){
  if(*wordCount >= *numWords-1){
	  //If we have more words than we initially allocated for, realloc the words
	int old = *wordCount;
	*numWords *= 2;
	char** tmp = (char**)realloc((char**)word, sizeof(char*) * (*numWords));
	word = tmp;
	int i;
	for(i = old; i < *numWords; i++){
	  word[i] = (char*)malloc(sizeof(char) * (*cPerWord));
	}
  }
  if(*charCount >= *cPerWord){
	/*
	  If we have more characters in a word than we initially allocated for,
	  go through and realloc each word to be the new size
	*/
	*cPerWord *= 2;
	int i;
	for(i = 0; i < *numWords; i++){
	  char* tmp = (char*)realloc(word[i], sizeof(char) * (*cPerWord));
		word[i] = tmp;
	}
  }
  return word;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  //error (1, 0, "command reading not yet implemented");
  commandNode *current = s->head;
  if(s->head != NULL){
	s->head = s->head->next;
	return current->command;
  }
  return NULL;
}

int getCommandStreamSize(command_stream_t s){
  return s->size;
}
