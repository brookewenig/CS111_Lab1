// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"
#include <error.h>
#include <errno.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

int
command_status (command_t c)
{
  return c->status;
}
void executingPipe(command_t c);

void
execute_command (command_t c, bool time_travel)
{ 
  // printf("made it here");
  int dumb = 0;
  switch(c->type){
  case SIMPLE_COMMAND:
	dumb = 1; //Switch statements are dumb, thus this must be the first statement

	int total = 0;
	char **currWord = c->u.word;
	while(currWord[0][0]){
	  total++;
	  currWord++;
	}

	c->u.word[total] = NULL;

	int inRedir, outRedir;
	int pid = fork();
	if(pid == 0){ 
	  if(c->input){
		inRedir = open(c->input, O_RDONLY);
		if(inRedir < 0)
		  error(1,0, "Failed to open the input");
		if(dup2(inRedir, 0) < 0)
		  error(1,0, "Failed to change the stdin");
	  }
	  if(c->output){
		outRedir = open(c->output, O_WRONLY|O_CREAT|O_TRUNC, 0666);
		if(outRedir < 0)
		  error(1,0, "Failed to open the output");
		if(dup2(outRedir, 1) < 0)
		  error(1,0, "Failed to change the stdout");
	  }
	  char exc[5];
	  strcpy(exc, "exec");
	  int ret = strcmp(c->u.word[0], exc);
	  if (ret == 0){
		execvp(c->u.word[1], (c->u.word+1));
	  }
	  else {
		execvp(c->u.word[0], (c->u.word));
	  }
	} 
	else if(pid > 0){
	  int status;
	  waitpid(pid,&status, 0);
	  int b = WEXITSTATUS(status);
	  if(c->input){
		close(inRedir);
		dup2(0,0);
	  }
	  if(c->output){
		close(outRedir);
		dup2(1,1);
	  }
	  c->status = b;
	} 
	else{
	  error(1,0, "Forking failed!");
	}
	break;
  case AND_COMMAND:
	execute_command(c->u.command[0], false);
	if(c->u.command[0]->status != 0){
	  c->status = -1;
	  return;
	}
	execute_command(c->u.command[1], false);
	c->status = c->u.command[1]->status;
	break;
  case OR_COMMAND:
	execute_command(c->u.command[0], false);
	if(c->u.command[0]->status == 0){
	  c->status = 0;
	  return;
	}
	execute_command(c->u.command[1], false);
	c->status = c->u.command[1]->status;
	break;
  case SEQUENCE_COMMAND:
	execute_command(c->u.command[0], false);
    execute_command(c->u.command[1], false);
    c->status = c->u.command[1]->status;
	break;
  case PIPE_COMMAND:
	executingPipe(c);	
	break;
  case SUBSHELL_COMMAND:
	dumb = 0;
	int inRed, outRed;
	if(c->input){
	  inRed = open(c->input, O_RDONLY);
	  if(inRed < 0)
		error(1,0, "Failed to open the input");
	  if(dup2(inRed, 0) < 0)
		error(1,0, "Failed to change the stdin");
	}
	if(c->output){
	  outRed = open(c->output, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	  if(outRed < 0)
		error(1,0, "Failed to open the output");
	  if(dup2(outRed, 1) < 0)
		error(1,0, "Failed to change the stdout");
	}
	execute_command(c->u.subshell_command,false);
	c->status = c->u.subshell_command->status;
	if(c->input){
	  close(inRedir);
	  dup2(0,0);
	}
	if(c->output){
	  close(outRedir);
	  dup2(1,1);
	}
	break;
  }
  //error (1, 0, "command execution not yet implemented");
}

void executingPipe(command_t c)
{
  pid_t returnedPid;
  pid_t firstPid;
  pid_t secondPid;
  int buffer[2];
  int eStatus;

  if ( pipe(buffer) < 0 )
	{
	  error (1, errno, "pipe was not created");
	}

  firstPid = fork();
  if (firstPid < 0)
	{
	  error(1, errno, "fork was unsuccessful");
	}
  else if (firstPid == 0) //child executes command on the right of the pipe
	{
	  close(buffer[1]); //close unused write end

	  //redirect standard input to the read end of the pipe
	  //so that input of the command (on the right of the pipe)
	  //comes from the pipe
	  if ( dup2(buffer[0], 0) < 0 )
		{
		  error(1, errno, "error with dup2");
		}
	  execute_command(c->u.command[1],false);
	  _exit(c->u.command[1]->status);
	}
  else 
	{
	  // Parent process
	  secondPid = fork(); //fork another child process
	  //have that child process executes command on the left of the pipe
	  if (secondPid < 0)
		{
		  error(1, 0, "fork was unsuccessful");
		}
	  else if (secondPid == 0)
		{
		  close(buffer[0]); //close unused read end
		  if(dup2(buffer[1], 1) < 0) //redirect standard output to write end of the pipe
			{
			  error (1, errno, "error with dup2");
			}
		  execute_command(c->u.command[0],false);
		  _exit(c->u.command[0]->status);
		}
	  else
		{
		  // Finishing processes
		  returnedPid = waitpid(-1, &eStatus, 0); //this is equivalent to wait(&eStatus);
		  //we now have 2 children. This waitpid will suspend the execution of
		  //the calling process until one of its children terminates
		  //(the other may not terminate yet)

		  //Close pipe
		  close(buffer[0]);
		  close(buffer[1]);

		  if (secondPid == returnedPid )
			{
			  //wait for the remaining child process to terminate
			  waitpid(firstPid, &eStatus, 0); 
			  c->status = WEXITSTATUS(eStatus);
			  return;
			}

		  if (firstPid == returnedPid)
			{
			  //wait for the remaining child process to terminate
			  waitpid(secondPid, &eStatus, 0);
			  c->status = WEXITSTATUS(eStatus);
			  return;
			}
		}
	}
}
