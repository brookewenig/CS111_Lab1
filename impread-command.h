#include <stdio.h>

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



int validComment(char c) {


}

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

