#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstring>
#include <string>
#include <map>
#include <cstdio>
#include <iostream>
#include <vector>
#include "tokens.h"
#include "lexer.h"
#include "parser.h"
using namespace std;

int main()
{
	freopen("data.txt", "r", stdin);
	freopen("out.txt", "w", stdout);
	
	Lexer lex(stdin);
	Parser parser(lex);
	
	parser.program();
	
	freopen("CON", "r", stdin);
	getchar();

	return 0;
}
