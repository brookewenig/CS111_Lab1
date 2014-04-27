 #include "queue.h"

 struct Node
 {
        command c;
        struct Node* next;
 }*start = NULL;

void delQueue()
{
     /* struct Node *temp, *var;
      if(var==end)
      {
             end = end->next;
             free(var);
      }
      else
      printf("\nQueue Empty");*/
}

void pushToFront(command com)
{
	struct Node *temp;
	temp = (struct Node *) malloc(sizeof(struct Node));
	temp->c = com;
	if (start == NULL) {
		start = temp;
		start->next = NULL;
		end = start;
		end->next = NULL;
	}
	else {
		temp->next = start;
		start = temp;
	}
}

void display()
{
     struct Node *var=start;
     if(var!=NULL)
     {
           printf("\nElements are as:  ");
           while(var!=NULL)
           {
                printf("\t%d",var->c);
                var=var->next;
           }
     printf("\n");
     } 
     else
     printf("\nQueue is Empty");
}

int main(int argc, char *argv[]) {
//    CommandStack *commandStack;
	int len;
	command com;
	start = NULL;
	char *foo = "ABC", *bar = "DEF", *baz = "Bleh", *snaz = "Meh";
	char *foobar[2];
	foobar[0] = foo;
	foobar[1] = bar;
	char *bazsnaz[2];
	bazsnaz[0] = baz;
	bazsnaz[1] = snaz;

	command c1, c2;
	c1.u.word = foobar;
	c2.u.word = bazsnaz;

//	commandStack = newCommandStack(100);
	pushToFront(c1);
	display();
	pushToFront(c2);
	display();
//	command top;
//	cmdPeek(commandStack, &top);
	//printf("%c",top.u.word[1][1]);
//	cmdPush(commandStack, c2);
//	cmdPeek(commandStack, &top);
	//printf("%c",top.u.word[1][1]);
//	len = cmdStackLen(commandStack);
//	printf("%d",len);
//	cmdPop(commandStack, &top);
	
//	len = cmdStackLen(commandStack);
//	printf("%d",len);
//	cmdPop(commandStack, &top);
//	len = cmdStackLen(commandStack);
//	printf("%d",len);
	//printf("%c",top.u.word[1][1]);
	
    // int i=0;
    // start=NULL;
    /* printf(" \n1. Push to Queue");
     printf(" \n2. Pop from Queue");
     printf(" \n3. Display Data of Queue");
     printf(" \n4. Exit\n");
     while(1)
     {
          printf(" \nChoose Option: ");
          scanf("%d",&i);
          switch(i)
          {
                case 1:
                {
                     command value;
                     printf("\nEnter a command to push into Queue: ");
                     scanf("%d",&value);
                     pushToFront(value); 
                     display();
                     break;
                }
                case 2:
                {
                     delQueue();
                     display();
                     break;
                }
                case 3:
                {
                     display();
                     break;
                }
                case 4:
                {
                     exit(0);
                }
                default:
                {
                     printf("\nwrong choice for operation");
                }
          }
     }*/
}
