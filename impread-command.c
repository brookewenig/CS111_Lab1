#include <stdio.h>
typedef enum {false=0, true=1} bool;
bool isNormal(char c)
{
        if (c >= 'A' && c <= 'Z' )
                return true;
        else if (c >='a' && c <='z')
                return true;
        else if (c >= '0' && c <= '9')
                return true;
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
                        return true;
                default:
                        return false;
          }
        }
}



bool validComment(char c) {


}

bool isOperator(char c) {
	switch(c) {
		case ';': 
			return true;
			break;
		case '|':
			return true;
			break;
		case '&':
			return true;
			break;
		case '(':
			return true;
			break;
		case ')':
			return true;
			break;
		default:
			return false;
			break;
	}
}

bool isRedirect(char c) {
	if (c == '<' || c == '>')
		return true;
	else
		return false;
}

