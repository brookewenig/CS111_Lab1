#include "commandstack.h"

int main(int argc, char *argv[]) {
    CommandStack *commandStack;
	int len;
	command com;
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

	commandStack = newCommandStack(100);
	cmdPush(commandStack, c1);
	command top;
	cmdPeek(commandStack, &top);
	//printf("%c",top.u.word[1][1]);
	cmdPush(commandStack, c2);
	cmdPeek(commandStack, &top);
	//printf("%c",top.u.word[1][1]);
	len = cmdStackLen(commandStack);
	printf("%d",len);
	cmdPop(commandStack, &top);
	
	len = cmdStackLen(commandStack);
	printf("%d",len);
	cmdPop(commandStack, &top);
	len = cmdStackLen(commandStack);
	printf("%d",len);
	//printf("%c",top.u.word[1][1]);

    /*printf("\nEnter text : ");
    fgets(in,MAX_LINE,stdin);
    *strchr(in,'\n') = '\0';
    if ((len = strlen(in)) > 0) {
        commandStack = newCommandStack(len);
        printf("\nstack len = %d",stackLen(commandStack));
        printf("\nstack max size = %d",stackMaxSize(commandStack));
        p = in;
        while (*p != '\0') push(commandStack,*p++);
        printf("\nstack len = %d\n",stackLen(commandStack));
        int count =0;
        while (peek(commandStack,&c) == true && count < 3)
                {
                         printf("Peek: %c\n",c);
                        count++;
                }
        while (pop(commandStack,&c) == true) printf("%c",c);
        if (pop(commandStack,&c) == false) {
            printf("\nstack empty - ok");
        } else {
            printf("\npopped from empty stack - error");
        }
        puts("");
    }
    delete(commandStack);*/
    return 0;
}

