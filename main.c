// UCLA CS 111 Lab 1 main program

#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <stdio.h>
#include <sys/types.h> //me add
#include "command.h"
#include "command-internals.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

static char const *program_name;
static char const *script_name;

typedef struct command command;
static void
usage (void)
{
  error (1, 0, "usage: %s [-pt] SCRIPT-FILE", program_name);
}

static int
get_next_byte (void *stream)
{
  return getc (stream);
}

/////////////////////////Inserting from here below
typedef struct GraphNode GraphNode;
typedef struct listNode listNode;
struct GraphNode{
	command_t command;
	GraphNode **before;
	pid_t pid;
};
struct listNode{
	GraphNode *g;
	char **readList;
	char **writeList;
};
 
typedef struct {
	listNode** no_dependencies;
	listNode** dependencies;
} DependencyGraph;

int numRead = 0;
int numWrite = 0;
	

int depNum = 0;
int noDepNum = 0;
int numCmd = 0;
int sizeListNodeArray = 20, numElemsNode = 10, numElemsNode1 = 10;
//have an array of listNodes
int h,j;
int processCommand(command_t cmd);
int procDependencies(command_t cmd);
bool instersectWW(listNode *re, listNode*wr);
bool instersectRW(listNode *re, listNode*wr);
void executeGraph(DependencyGraph *dependencyGraph);
void executeNoDependencies(listNode **no_dependencies);
void executeDependencies(listNode **dependencies);
listNode* checkNAForRealloc(const int *nodeCount, int *maxNodes, listNode* array);
char** checkListForRealloc(const int *wordCount, int *numWords, char** word);
listNode *listNodeArray;
DependencyGraph graph;

int
main (int argc, char **argv)
{
  int command_number = 1;
  bool print_tree = false;
  bool time_travel = false;
  program_name = argv[0];

  for (;;)
    switch (getopt (argc, argv, "pt"))
      {
      case 'p': print_tree = true; break;
      case 't': time_travel = true; break;
      default: usage (); break;
      case -1: goto options_exhausted;
      }
 options_exhausted:;

  // There must be exactly one file argument.
  if (optind != argc - 1)
    usage ();

  script_name = argv[optind];
  FILE *script_stream = fopen (script_name, "r");
  if (! script_stream)
    error (1, errno, "%s: cannot open", script_name);
  command_stream_t command_stream =
    make_command_stream (get_next_byte, script_stream);

  command_t last_command = NULL;
  command_t command;
  sizeListNodeArray = getCommandStreamSize(command_stream);
//added here
listNodeArray = (listNode*) malloc(sizeof(listNode)*sizeListNodeArray);
 int h;//,j;
for (h=0; h<sizeListNodeArray; h++) {
  listNodeArray[h].readList = (char **)calloc(numElemsNode, sizeof(char *));
  listNodeArray[h].writeList = (char **)calloc(numElemsNode1, sizeof(char *));
  listNodeArray[h].g = (GraphNode*)malloc(sizeof(GraphNode));
/*	for (j=0; j < numElemsNode; j++) {
		listNodeArray[h].readList[j] = (char *)malloc(sizeof(char)*cPerWord);
		listNodeArray[h].writeList[j] = (char *)malloc(sizeof(char)*cPerWord);
	}*/
}

  //to here
 if(time_travel){
   graph.no_dependencies = malloc(sizeof(listNode*)*(sizeListNodeArray-1));
   graph.dependencies = malloc(sizeof(listNode*)*(sizeListNodeArray-1));
 }
 while ((command = read_command_stream (command_stream))) {
   if (print_tree){
	 printf ("# %d\n", command_number++);
	 print_command (command);
   }
   else{
	 if(!time_travel){
	   last_command = command;
	   execute_command (command, time_travel);
	 }
	 else{
	   numRead = 0;
	   numWrite = 0;
	   processCommand(command);
	   procDependencies(command);
	   numCmd++;
	 }
   }
 }
 if(time_travel){
   executeGraph(&graph);
   int i;
   int status;
   for(i=0; i < noDepNum; i++){
	 if(graph.no_dependencies[i]->g->pid != -1){
	   waitpid(graph.no_dependencies[i]->g->pid, &status, 0);
	 }
   }
   for(i=0; i < depNum; i++){
	 if(graph.dependencies[i]->g->pid != -1){
	   waitpid(graph.dependencies[i]->g->pid, &status, 0);
	 }
   }
 }
 return 0;
}

int processCommand(command_t cmd) {
	if (cmd->type == SIMPLE_COMMAND) {
		char **currWord = cmd->u.word;
		currWord++; //don't want to grab the actual command
		//NOTE: remember to parse for options so don't include them
		listNodeArray = checkNAForRealloc(&numCmd, &sizeListNodeArray, listNodeArray);
		while(currWord[0][0]) {
			if (currWord[0][0] == '-'){
				currWord++;
				continue;
			}
			listNodeArray[numCmd].readList = checkListForRealloc(&numRead,&numElemsNode,listNodeArray[numCmd].readList);
			listNodeArray[numCmd].readList[numRead] = currWord[0];
			currWord++;
			numRead++;
		}
		if (cmd->input) {
		  listNodeArray[numCmd].readList = checkListForRealloc(&numRead,&numElemsNode,listNodeArray[numCmd].readList);
		  listNodeArray[numCmd].readList[numRead] = cmd->input;
		  numRead++;
		}
		if (cmd->output) {
		  listNodeArray[numCmd].writeList = checkListForRealloc(&numWrite,&numElemsNode1,listNodeArray[numCmd].writeList);
		  listNodeArray[numCmd].writeList[numWrite] = cmd->output;
		  numWrite++;
		}
	}
	else if(cmd->type == SUBSHELL_COMMAND) {
	  listNodeArray = checkNAForRealloc(&numCmd, &sizeListNodeArray, listNodeArray);
	  if (cmd->input) {
		listNodeArray[numCmd].readList = checkListForRealloc(&numRead,&numElemsNode,listNodeArray[numCmd].readList);
		listNodeArray[numCmd].readList[numRead] = cmd->input;
		numRead++;	
	  }
	  if (cmd->output) {
		listNodeArray[numCmd].writeList = checkListForRealloc(&numWrite,&numElemsNode1,listNodeArray[numCmd].writeList);
		listNodeArray[numCmd].writeList[numWrite] = cmd->output;
		numWrite++;	
	  }
	  processCommand(cmd->u.subshell_command);
	}
	else {
	  processCommand(cmd->u.command[0]);
	  processCommand(cmd->u.command[1]);
	}
	return 0;
}

int procDependencies(command_t com){
  int i;
  listNodeArray[numCmd].g->before = NULL;
  listNodeArray[numCmd].g->command = com;
  listNodeArray[numCmd].g->pid = -1;
  int bCount = 0;
  for(i = 0; i < numCmd; i++){
	if(instersectRW(&listNodeArray[i],&listNodeArray[numCmd]) || 
					   instersectWW(&listNodeArray[i],&listNodeArray[numCmd]) || 
					   instersectRW(&listNodeArray[numCmd],&listNodeArray[i])){
	  if(listNodeArray[numCmd].g->before == NULL){
		listNodeArray[numCmd].g->before = calloc((sizeListNodeArray-1), sizeof(char*));
	  }
	  listNodeArray[numCmd].g->before[bCount] = listNodeArray[i].g;
	  bCount++;
	}
  }
  if(listNodeArray[numCmd].g->before == NULL){
	graph.no_dependencies[noDepNum] = &listNodeArray[numCmd];
	noDepNum++;
  }
  else{
	graph.dependencies[depNum] = &listNodeArray[numCmd];
	depNum++;
  }
  return 0;
}

bool instersectRW(listNode *re, listNode*wr){
  char **r = re->readList;
  while(r[0]){
	char **w = wr->writeList;
	while(w[0]){
	  if(*r[0] == *w[0])
		return true;
	  w++;
	}
	r++;
  }
  return false;
}

bool instersectWW(listNode *re, listNode*wr){
  char **r = re->writeList;
  while(r[0]){
	char **w = wr->writeList;
	while(w[0]){
	  if(*(r[0]) == *w[0])
		return true;
	  w++;
	}
	r++;
  }
  return false;
}


void executeGraph(DependencyGraph *dependencyGraph) {
	executeNoDependencies(dependencyGraph->no_dependencies);
	executeDependencies(dependencyGraph->dependencies);
}

void executeNoDependencies(listNode **no_dependencies) {
	int i;
	for (i=0; i < noDepNum; i++) {
		pid_t pid = fork();
		if (pid ==0) {
		  execute_command(no_dependencies[i]->g->command,false);
			_exit(no_dependencies[i]->g->command->status);
		}
		if (pid >0)
			no_dependencies[i]->g->pid = pid;
	}
}

void executeDependencies(listNode **dependencies) {
  int i;
  for (i=0; i < depNum; i++){ //does all of the polling before it does any blocking
  GraphNode **bef = dependencies[i]->g->before;
  loop_label:
	while(bef[0]){
	  if (bef[0]->pid == -1) {
		goto loop_label;
	  }
	  bef++;
	}
	int status;
	bef = dependencies[i]->g->before;
	while(bef[0]){
	  waitpid(bef[0]->pid, &status, 0);
	  bef[0]->pid = -1; // This indicates we've already waited on this PID. We'll wait on GraphNodes with non -1 pids later
	  bef++;
	}
	pid_t pid = fork();
	if (pid ==0){
	  execute_command(dependencies[i]->g->command, false);
	  _exit(dependencies[i]->g->command->status);
	}
	if (pid >0) {
	  dependencies[i]->g->pid = pid;
	}
  }
}

listNode* checkNAForRealloc(const int *nodeCount, int *maxNodes, listNode* array){
  if (*nodeCount >= *maxNodes){
	int initial = *maxNodes;
	//If we have more words than we initially allocated for, realloc the words
	*maxNodes *= 2;
	listNode* tmp = NULL;
	tmp = (listNode*)realloc(array, sizeof(listNode) * (*maxNodes));
	array = tmp;
	int i, j;
	for(i = 0, j = 0; i+j < initial;){
	  if(array[i+j].g->before == NULL){
		graph.no_dependencies[i]->g = array[i+j].g; 
		i++;
	  }
	  else{
		graph.dependencies[j]->g = array[i+j].g;
		j++;
	  }
	}
	for (i=initial; i<*maxNodes; i++) {
	  array[i].readList = (char **)calloc(numElemsNode, sizeof(char *));
	  array[i].writeList = (char **)calloc(numElemsNode1, sizeof(char *));
	  array[i].g = (GraphNode*)malloc(sizeof(GraphNode));
	  /*  for (j=0; j < numElemsNode; j++) {
        listNodeArray[h].readList[j] = (char *)malloc(sizeof(char)*cPerWord);
        listNodeArray[h].writeList[j] = (char *)malloc(sizeof(char)*cPerWord);
		}*/
	}
  }
  return array;
}

char** checkListForRealloc(const int *wordCount, int *numWords, char** word){
  if(*wordCount >= *numWords){
	//If we have more words than we initially allocated for, realloc the words
	*numWords *= 2;
	char** tmp = realloc(word, sizeof(char*) * (*numWords));
	word = tmp;
  }
  return word;
}
