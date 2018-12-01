#include <cstring>
#include <string>
#include <cstdio>
#include <map>
#include <set>
#include <vector>
#include "tokens.h"
#include "lexer.h"
#include "parser.h"

extern bool follow[50][50];

Parser::Parser(Lexer l)
{
	lex = l;
	look = (Token)lex.nextToken();
}

Parser::~Parser()
{
}

void Parser::program()
{
	prog();
	if (lex.token != EOF) {
		get_error();
	}
}

inline bool Parser::match(Tag t)
{
	printf("%d ", (int)t);
	if (look == t) move();
	else 
	{
		get_error();
		if (look == EOF) return true;
		while ( look != t && !follow[(int)t][(int)look] ) move();
		if (look == t) move();
	}
		
	return true;
}

inline void Parser::move()
{
	while ((look = (Token)lex.nextToken()) == ERROR);
}

void Parser::get_error()
{
	fprintf(stderr, "Line:%3d, Col:%3d: syntax error! \n", lex.get_line(), lex.get_col());
	if (lex.token == EOF) {
		fprintf(stderr, "File END! \n");
		//exit(1);
	}
}

void Parser::prog()
{
	print_msg("prog");
	match(PROGRAM);
	match(ID);
	match(SEMICOLON);
	block();
}

void Parser::block()
{
	print_msg("block");
	if (look == CONST)	condecl();
	if (look == VAR)	vardecl();
	if (look == PROCEDURE)	proc();
	body();
}

void Parser::condecl()
{
	print_msg("condecl");
	match(CONST);
	constexp();
	while (look == COMMA) {
		move();
		constexp();
	}
	match(SEMICOLON);
}

void Parser::constexp()
{
	print_msg("constexp");
	match(ID); match(ASSIGN); match(NUM);
}

void Parser::vardecl()
{
	print_msg("vardecl");
	match(VAR);
	match(ID);
	while (look == COMMA) {
		move();
		match(ID);
	}
	match(SEMICOLON);
}

void Parser::proc()
{
	print_msg("proc");

	match(PROCEDURE); match(ID);
	match(LPAR);
	if (look == ID) {
		move();
		while (look == COMMA) {
			move();
			match(ID);
		}
	}
	match(RPAR);
	match(SEMICOLON);
	block();
	while (look == SEMICOLON) {
		move();
		proc();
	}
}

void Parser::body()
{
	print_msg("body");

	match(BEGIN);
	statement();
	while (look == SEMICOLON) {
		move();
		statement();
	}
	match(END);
}

void Parser::statement()
{
	print_msg("statement");

	switch (look) {
	case ID: move();
		match(ASSIGN); exp();
		break;
	case IF: move();
		lexp(); match(THEN); statement();
		if (look == ELSE) { move();
			statement();	
		}
		break;
	case WHILE: move();
		lexp(); match(DO); statement();
		break;
	case CALL: move();
		match(ID); match(LPAR);
		if (is_exp(look)) { exp();
			while (look == COMMA) { move();
				exp();
			}
		}
		match(RPAR);
		break;
	case BEGIN:
		body();
		break;
	case READ: move();
		match(LPAR);
		match(ID);
		while (look == COMMA) { move();
			match(ID);
		}
		match(RPAR);
		break;
	case WRITE: move();
		match(LPAR);
		exp();
		while (look == COMMA) { move();
			exp();
		}
		match(RPAR);
		break;
	default:
		;
	}
			
}

void Parser::lexp()
{
	print_msg("lexp");

	switch (look) {
	case ODD: move();
		exp();
		break;
	case ADD:
	case SUB:
	case ID:
	case NUM:
	case LPAR:
		exp(); lop(); exp();
		break;
	default:
		;
	}	
}

void Parser::exp()
{
	print_msg("exp");

	if (is_aop(look)) { move(); }
	term();
	while (is_aop(look)) { move();
		term();
	}	
}

void Parser::term()
{
	print_msg("term");

	factor();
	while (is_mop(look)) { move();
		factor();
	}	
}

void Parser::factor()
{
	print_msg("factor");

	switch (look) {
	case ID: move();
		break;
	case NUM: move();
		break;
	case LPAR: move();
		exp(); match(RPAR);
		break;
	default:
		get_error();
		;
	}
}

inline void Parser::lop()
{
	print_msg("lop");

	if (is_lop(look)) move();
	else ;
}

inline void Parser::aop()
{
	print_msg("aop");

	if (is_aop(look)) move();
	else ;
}

inline void Parser::mop()
{
	print_msg("mop");

	if (is_mop(look)) move();
	else ;
}

inline bool Parser::is_lop(Tag t)
{
	return EQ <= t && t<= GE;
}

bool Parser::is_exp(Tag t)
{
	return is_aop(t) || t == ID || t == NUM || t == LPAR;
}

void Parser::print_msg(std::string s)
{
	printf("%s\n", s.c_str());
}

inline bool Parser::is_aop(Tag t)
{
	return t == ADD || t == SUB;
}

inline bool Parser::is_mop(Tag t)
{
	return t == MUL || t == DIV;
}