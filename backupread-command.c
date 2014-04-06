// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  REMEMBER TO ADD FUNCTION DECLARATIONS UP HERE SILLIES IM TOO LAZY RIGHT NOW
  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

struct commandNode{
	struct command* command; //stuct command* defined in command-internals.h 
	struct commandNode *next;
};

typedef struct command_stream{
	struct commandNode *head;
	struct commandNode *tail;
} command_stream;
/* Took care of - FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

enum readMode{
  SIMPLE_MODE,
  INPUT_MODE,
  OUTPUT_MODE,
} mode;

command_stream_t make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  command_stream_t cStream;
  
  mode = SIMPLE_MODE;

  //currNode will be our current node, prev will be our previous node
  commandNode *currNode = checked_malloc(sizeof(commandNode));
  commandNode *prevNode = checked_malloc(sizeof(commandNode));

  /*
	Here we init currNode's char** to be 4 words long,
	each word consisting of 4 characters initially
   */
  int numWords = 4, cPerWord = 4;
  int wordCount = 0, charCount = 0;
  int inSize = 4, outSize = 4;
  int inpCount = 0, outCount = 0;

  initNode(currNode, numWords, cPerWord, inSize, outSize);

  int currChar = get_next_byte(get_next_byte_argument);
  int prevChar = NULL;
  
  /*
	Explanation of the modes: If we're in input mode, we need to read NORMAL tokens into
	the input field of the command, rather than the word filed, likewise for output. The special
	token merely indicates we're changing to that mode. We then want to read NORMAL tokens into
	whatever until we hit something that tells us to stop. Which would be a special token or space 
	I believe. The spec says that a command is a simple command followed by a redirect followed by
	a word, which leads me to believe that in "cat < foo bar" the bar is not part of the "cat < foo"
	command. This however is slightly confusing, because then is "cat < foo bar" even valid syntax?
	It certainly runs in the shell, producing the contents of bar, so I'm not sure. Got to go now
   */

  while(currChar != EOF){
	if(isWord(currChar)){
	  switch mode{
		case SIMPLE_MODE:
		  //If we hit a normal and the prev was space, new word, reset the charCount
		  if(prevChar == ' ' && currChar != ' '){
			wordCount++;
			charCount = 0;
		  }
		  //We'll add the character to the current command's current word after checking for reallocation
		  checkWordForRealloc(wordCount,charCount,numWords,cPerWord, currNode);
		  currNode->command.word[wordCount][charCount] = currChar;
		  charCount++;
		  break;
		case INPUT_MODE:
		  if(prevChar == ' ' && currChar != ' '){
            inpCount = 0;
			mode = SIMPLE_MODE;
          }
	  }
	}
	//NOTE: We might want to turn isSpecial into just interpretSpecial, because here
	//We're going to need to do another switch to act for each value anyway
	else if(isSpecialToken(currChar, prevChar)){
	  switch(currChar){
	    case '<':
		  checkInForRealloc(inpCount, inSize, currNode);
		  mode = INPUT_MODE;
		  break;
	  }
	}
	
	currChar = get_next_byte(get_next_byte_argument);
  }
  return 0;
}

void initNode(commandNode *currNode, const int &numWords, const int &cPerWord, 
			  const int &inSize, const int &outSize){
  currNode->command.word = checked_malloc(sizeof(char*) * numWords);
  for(int i = 0; i < numWords; i++){
    currNode->command.word[i] = checked_malloc(sizeof(char) * cPerWord);
  }
  currNode->command.input = checked_malloc(sizeof(char) * inSize);
  currNode->command.output = checked_malloc(sizeof(char) * outSize);
}

void checkInForRealloc(int inpCount, int &inSize, commandNode *currNode){
  if(inpCount >= inSize){
    /*
      If we have more characters in the input than we initially allocated for
	  realloc the input
	*/
    inSize *= 2;
	char* tmp = checked_realloc(currNode->command.input, sizeof(char) * inSize);
	currNode->command.input = tmp;
  }
}

void checkOutForRealloc(int outCount, int &outSize, commandNode *currNode){
  if(outCount >= outSize){
	/*
      If we have more characters in the output than we initially allocated for
      realloc the output
	*/
	outSize *= 2;
	char* tmp = checked_realloc(currNode->command.output, sizeof(char) * outSize);
	currNode->command.output = tmp;
  }
}


void checkWordForRealloc(int wordCount, int charCount, int &numWords, int &cPerWord, commandNode *currNode){
  if(wordCount >= numWords){
	  //If we have more words than we initially allocated for, realloc the words
	  numWords *= 2;
	  char** tmp = checked_realloc(currNode->command.word, sizeof(char*) * numWords);
	  currNode->command.word = tmp;
  }
  if(charCount >= cPerWord){
	/*
	  If we have more characters in a word than we initially allocated for,
	  go through and realloc each word to be the new size
	*/
	cPerWord *= 2;
	for(int i = 0; i < numWords; i++){
	  char* tmp = checked_realloc(currNode->command.word[i], sizeof(char) * cPerWord);
		currNode->command.word[i] = tmp;
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



bool isWord(char c)
{
	if (c >= 'A' && c <= 'Z' )
		return true;
	else if (c >='a' && c <='z')
		return true;
	else if (c >= '0' && c <= '9')
		return true;
	else {
	switch (c) 
	{
		case '!':
		case '%':
		case '+':
		case ',':
		case '-':
		case '.':
		case '/':
		case ':':
		case '@':
		case '^':
		case '_':
			return true;
		default:
			return false;
	}
	}
}

bool validComment(char c) {

}

/*
  Brooke: I added in the prev to check for the && and || cases
*/
bool isSpecialToken(char c, char prev) {
	switch (c)
	{
		case ';':
	//	case '|':
	//	check if || or |
		case '&':
		  if(prev!='&')
			return false
		case '|':
		  if(prev!='|')
			return false;
		case '(':
		case ')':
		case '<':
		case '>':
			return true;
		default:
			return false;
	}
}
