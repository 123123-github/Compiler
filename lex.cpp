#include <cstdio>
#include <iostream>
#include <cstring>
#include <map>
#include <vector>
#include <string>
using namespace std;

const int maxn = 10;
const int NUM = 0, ID = 1, TAG = 2, SYMBOL = 3, LIMIT = 4;
string get_type[maxn] = { "NUM", "ID", "TAG", "SYMBOL", "LIMIT"};

char peek = ' ';
int line = 1, col = 1;
map<string, int> words;

void retract() { ungetc(peek, stdin); col--; }

bool readch()
{
	peek = getchar(); col++;
	if (peek == EOF) return false;
	else return true;
}

bool readch(char ch)
{
	readch();
	if (ch != peek) {
		retract();
		return false;	
	}
	peek = ' ';
	return true;
}

void add_word(int type, const string& s) { words[s] = type; }

void init()
{
	words.clear();
	
	// reserve word
	add_word(TAG, "program");
	add_word(TAG, "const");
	add_word(TAG, "var");
	add_word(TAG, "procedure");
	add_word(TAG, "begin");
	add_word(TAG, "end");	
	add_word(TAG, "if");
	add_word(TAG, "then");
	add_word(TAG, "else");
	add_word(TAG, "while");
	add_word(TAG, "do");
	add_word(TAG, "call");
	add_word(TAG, "read");
	add_word(TAG, "write");
	add_word(TAG, "odd");
	
	// reserve symbol
	add_word(SYMBOL, "+");
	add_word(SYMBOL, "-");
	add_word(SYMBOL, "*");
	add_word(SYMBOL, "/");
	add_word(SYMBOL, ":=");
	add_word(SYMBOL, "=");
	add_word(SYMBOL, "<");
	add_word(SYMBOL, "<>");
	add_word(SYMBOL, "<=");
	add_word(SYMBOL, ">");
	add_word(SYMBOL, ">=");
	add_word(SYMBOL, "(");
	add_word(SYMBOL, ")");
	
	// reserve limit
	add_word(LIMIT, ",");
	add_word(LIMIT, ";");
}


void print_token(int v)
{
	printf("<NUM,\t%d>", v);
	putchar('\n');
}

void print_token(const string& s)
{
	if (!words.count(s)) add_word(ID, s);
	printf("<%s,\t\"%s\">", get_type[words[s]].c_str(), s.c_str());
	putchar('\n');
}

void print_error()
{
	printf("Line %d, Col: %d: [Error] stray '%c' in program.", line, col, peek);
	putchar('\n');
	peek = ' ';
}

int main()
{
	freopen("data.txt", "r", stdin);
	freopen("out.txt", "w", stdout);
	
	init();
	
	while ( peek != EOF )
	{
		// jump BLANK
		do
		{
			if (peek == '\n') { line++; col = 1; }
			else if (isblank(peek)) continue;
			else break;
		} while (readch());
		
		// get LEX
		if ( isdigit(peek) )			// deal with NUM
		{
			int v = 0;
			do { v = v*10 + (peek-'0'); } while ( readch() && isdigit(peek) );
			print_token(v);
		}
		else if ( isalpha(peek) )		// deal with ID or TAG
		{
			string s;
			do { s.push_back(peek); } while ( readch() && isalnum(peek) );
			print_token(s);
		}
		else if ( peek != EOF)			// deal with OTHER SYMBOL
		{
			switch(peek) {
			case ':':
				if (readch('=')) print_token(":=");
				break;
			case '<':
				if (readch('>')) print_token("<>");
				else if (readch('=')) print_token("<=");
				else print_token("<");
				break;
			case '>':
				if (readch('=')) print_token(">=");
				else print_token(">");
				break;
			case '=':
				print_token("="); break;
			case '+':
				print_token("+"); break;
			case '-':
				print_token("-"); break;
			case '*':
				print_token("*"); break;
			case '/':
				print_token("/"); break;
			case '(':
				print_token("("); break;
			case ')':
				print_token(")"); break;
			case ',':
				print_token(","); break;
			case ';':
				print_token(";"); break;	
			default:
				print_error();
			}
			peek = ' ';
		}
	}
	
	return 0;
}
