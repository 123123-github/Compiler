#define _CRT_SECURE_NO_WARNINGS
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <map>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "Tag.h"
#include "lexer.h"
#include "parser.h"
using namespace std;

int main()
{
	freopen("data.txt", "r", stdin);
	freopen("out.txt", "w", stdout);

	ifstream infile;
//	ofstream outfile;
//	infile.unget();
//	infile.open("data.txt");
//	assert(infile.fail());
//	outfile.open("out.txt");
//	assert(outfile.fail());

	Lexer lex(stdin);
	Parser parser(lex);
	
	parser.program();
	
//	infile.close();
//	outfile.close();
	
	freopen("CON", "r", stdin);
	getchar();

	return 0;
}
