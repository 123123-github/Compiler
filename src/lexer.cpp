#include <map>
#include <vector>
#include <string>
#include <cstring>
#include "lexer.h"
#include "tag.h"
using std::string;

// constructor & destructor
Lexer::Lexer(FILE* infile)
{
	in = infile;
	
	token = 0;
	peek = ' ';
	line = col = 1;
	
	words.clear();
	// reserve word
	add_word(PROGRAM,	"program");
	add_word(CONST,		"const");
	add_word(VAR,		"var");
	add_word(PROCEDURE,	"procedure");
	add_word(BEGIN,		"begin");
	add_word(END,		"end");
	add_word(IF,		"if");
	add_word(THEN,		"then");
	add_word(ELSE,		"else");
	add_word(WHILE,		"while");
	add_word(DO,		"do");
	add_word(CALL,		"call");
	add_word(READ,		"read");
	add_word(WRITE,		"write");
	add_word(ODD,		"odd");
	// reserve symbol
	add_word(ADD,		"+");
	add_word(SUB,		"-");
	add_word(MUL,		"*");
	add_word(DIV,		"/");
	add_word(ASSIGN,	":=");
	add_word(EQ,		"=");
	add_word(LESS,		"<");
	add_word(NE,		"<>");
	add_word(LE,		"<=");
	add_word(GREATER,	">");
	add_word(GE,		">=");
	add_word(LPAR,		"(");
	add_word(RPAR,		")");
	// reserve limit
	add_word(COMMA,		",");
	add_word(SEMICOLON,	";");
}

Lexer::~Lexer()
{
	in = NULL;	
}

// private func
inline bool Lexer::readch()
{
	peek = getc(in); col++;
	if (peek == EOF) return false;
	else return true;
}

inline bool Lexer::readch(char ch)
{
	readch();
	if (ch != peek) {
		retract();
		return false;	
	}
	peek = ' ';
	return true;
}

inline void Lexer::retract()
{
	ungetc(peek, in); col--;
}

inline void Lexer::add_word(int type, const string& s) 
{
	words[s] = type;
}

inline void Lexer::get_token(int v)
{
	token = NUM;
	tokenValue = v;
}

inline void Lexer::get_token(const string& s)
{
	tokenWord = s;
	if (!words.count(s)) {
		add_word(token = ID, s);		// is reserve word
	}
	else {
		token = words[s];								// isn't 
	}
}

inline void Lexer::get_error()
{
	token = ERROR;
	fprintf(stderr, "Line:%3d, Col:%3d: [Error] stray '%c' in program.\n", line, col, peek);
}

// public func
int Lexer::get_line()
{
	return line;
}

int Lexer::get_col()
{
	return col;
}

int Lexer::nextToken()
{	
	if ( peek != EOF )
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
			get_token(v);
		}
		else if ( isalpha(peek) )		// deal with ID or TAG
		{
			string s;
			do { s.push_back(peek); } while ( readch() && isalnum(peek) );
			get_token(s);
		}
		else if ( peek != EOF )			// deal with OTHER SYMBOL
		{
			switch(peek) {
			case ':':
				if (readch('=')) get_token(":=");
				break;
			case '<':
				if (readch('>')) get_token("<>");
				else if (readch('=')) get_token("<=");
				else get_token("<");
				break;
			case '>':
				if (readch('=')) get_token(">=");
				else get_token(">");
				break;
			case '=':
				get_token("="); break;
			case '+':
				get_token("+"); break;
			case '-':
				get_token("-"); break;
			case '*':
				get_token("*"); break;
			case '/':
				get_token("/"); break;
			case '(':
				get_token("("); break;
			case ')':
				get_token(")"); break;
			case ',':
				get_token(","); break;
			case ';':
				get_token(";");	break;
			default:
				get_error(); break;
			}
			peek = ' ';
		}
		else token = EOF;
	}
	else token = EOF;
	
	return token;
}
