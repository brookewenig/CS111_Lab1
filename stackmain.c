#include "stack.h"

int main(int argc, char *argv[]) {
    char in[MAX_LINE],c,*p;
    CharStack *charStack;
    int len;

    printf("\nEnter text : ");
    fgets(in,MAX_LINE,stdin);
    *strchr(in,'\n') = '\0';
    if ((len = strlen(in)) > 0) {
        charStack = newCharStack(len);
        printf("\nstack len = %d",stackLen(charStack));
        printf("\nstack max size = %d",stackMaxSize(charStack));
        p = in;
        while (*p != '\0') push(charStack,*p++);
        if (push(charStack,'X') == false) {
            printf("\nstack is full - ok");
        } else {
            printf("\npushed to full stack - error");
        }
        printf("\nstack len = %d\n",stackLen(charStack));
        int count =0;
        while (peek(charStack,&c) == true && count < 3)
                {
                         printf("Peek: %c\n",c);
                        count++;
                }
        while (pop(charStack,&c) == true) printf("%c",c);
        if (pop(charStack,&c) == false) {
            printf("\nstack empty - ok");
        } else {
            printf("\npopped from empty stack - error");
        }
        puts("");
    }
    delete(charStack);
    return 0;
}

