#include <map>
#include <vector>
#include <string>
#include <cstring>
#include "lexer.h"
#include "tag.h"
#include "token.h"
using std::string;

// constructor & destructor
Lexer::Lexer(FILE* infile)
{
	in = infile;
	
	token = Token(Tag::ERROR, 0, "");
	peek = ' ';
	line = col = 1;
	
	words.clear();
	// reserve word
	add_word((int)Tag::PROGRAM,	"program");
	add_word((int)Tag::CONST,		"const");
	add_word((int)Tag::VAR,		"var");
	add_word((int)Tag::PROCEDURE,	"procedure");
	add_word((int)Tag::BEGIN,		"begin");
	add_word((int)Tag::END,		"end");
	add_word((int)Tag::IF,		"if");
	add_word((int)Tag::THEN,		"then");
	add_word((int)Tag::ELSE,		"else");
	add_word((int)Tag::WHILE,		"while");
	add_word((int)Tag::DO,		"do");
	add_word((int)Tag::CALL,		"call");
	add_word((int)Tag::READ,		"read");
	add_word((int)Tag::WRITE,		"write");
	add_word((int)Tag::ODD,		"odd");
	// reserve symbol
	add_word((int)Tag::ADD,		"+");
	add_word((int)Tag::SUB,		"-");
	add_word((int)Tag::MUL,		"*");
	add_word((int)Tag::DIV,		"/");
	add_word((int)Tag::ASSIGN,	":=");
	add_word((int)Tag::EQ,		"=");
	add_word((int)Tag::LESS,		"<");
	add_word((int)Tag::NE,		"<>");
	add_word((int)Tag::LE,		"<=");
	add_word((int)Tag::GREATER,	">");
	add_word((int)Tag::GE,		">=");
	add_word((int)Tag::LPAR,		"(");
	add_word((int)Tag::RPAR,		")");
	// reserve limit
	add_word((int)Tag::COMMA,		",");
	add_word((int)Tag::SEMICOLON,	";");
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
	token = Token(Tag::NUM, v, "");
}

inline void Lexer::get_token(const string& s)
{
	token.lexeme = s;
	if (!words.count(s)) {
		token.tag = Tag::ID;
	}
	else {
		token.tag = (Tag)words[s];
	}
}

inline void Lexer::get_error()
{
	token = Tag::ERROR;
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

Token Lexer::nextToken()
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
		else token.tag = Tag::OVER;
	}
	else token.tag = Tag::OVER;
	
	return token;
}
