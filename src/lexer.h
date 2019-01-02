#include "token.h"
class Lexer
{
public:
	Lexer(FILE* infile = stdin);
	~Lexer();

public:
	Token token;
	Token nextToken();
	int get_line();
	int get_col();		// file pos

private:
	FILE* in;
private:
	char peek;
	int line, col;
	std::map<std::string, int> words;
private:
	bool readch();
	bool readch(char ch);
	void retract();							// file read
	void get_error();
	void get_token(int v);
	void get_token(const std::string& s);	// return Token
	void add_word(int type, const std::string& s);
};

//int token;
//int tokenValue;
//std::string tokenWord;