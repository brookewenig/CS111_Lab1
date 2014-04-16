#include <stdio.h>
#include <error.h>

int isNormal(char c)
{
        if (c >= 'A' && c <= 'Z' )
                return 1;
        else if (c >='a' && c <='z')
                return 1;
        else if (c >= '0' && c <= '9')
                return 1;
        else {
          switch (c) {
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
                        return 1;
                default:
                        return 0;
          }
        }
}


int validComment(char currChar, char prevChar) {
	if (currChar == '#') {
		if (!isNormal(prevChar)) {
			return 1; //1 means to keep reading until hit newline
		}
		else
			return 0; //invalid comment
	}
	return 0; //it is not a comment
}

//Need to test this function

int isOperator(char c) {
	switch(c) {
		case ';': 
			return 1;
			break;
		case '|':
			return 1;
			break;
		case '&':
			return 1;
			break;
		case '(':
			return 1;
			break;
		case ')':
			return 1;
			break;
		default:
			return 0;
			break;
	}
}

int isRedirect(char c) {
	if (c == '<' || c == '>')
		return 1;
	else
		return 0;
}

int isSpecial(char c) {
	return (isOperator(c) || isRedirect(c));
}
