#include <cstring>
#include <string>
#include <cstdio>
#include <map>
#include <set>
#include <vector>
#include "tag.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"
#include "env.h"

extern bool follow[50][50];

Parser::Parser(Lexer l)
{
	lex = l;
	look = (Tag)lex.nextToken().tag;
}

Parser::~Parser()
{
}

void Parser::program()
{
	prog();
	
	if (lex.token.tag != OVER) {
		get_error();
	}
}

inline void Parser::move()
{
	while ((look = (Tag)lex.nextToken().tag) == ERROR);
}

inline bool Parser::match(Tag t)
{
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

void Parser::get_error()
{
	fprintf(stderr, "Line:%3d, Col:%3d: syntax error! \n", lex.get_line(), lex.get_col());
	if (lex.token.tag == OVER) {
		fprintf(stderr, "File END! \n");
		//exit(1);
	}
}

void Parser::prog()
{
	Token temp;
	match(PROGRAM); temp = lex.token; match(ID); match(SEMICOLON);
	
	table_list.enter_proc(temp.lexeme);	// new symbol table
	block();
	table_list.leave_proc();	// goback symbol table
}

void Parser::block()
{
	if (look == CONST)	condecl();
	if (look == VAR)	vardecl();
	if (look == PROCEDURE) {
		proc();
	}
	body();
}

void Parser::condecl()
{
	match(CONST);
	constexp();
	while (look == COMMA) { move();
		constexp();
	}
	match(SEMICOLON);
}

void Parser::constexp()
{
	Token temp1, temp2;
	temp1 = lex.token; match(ID); match(ASSIGN); temp2 = lex.token; match(NUM);
	// --- add to symbol table ---
	table_list.save(Tag::CONST_TYPE, temp1.lexeme, temp2.value);
}

void Parser::vardecl()
{
	Token temp;
	match(VAR); temp = lex.token; match(ID);
	// --- add to symbol table ---
	table_list.save(Tag::VAR_TYPE, temp.lexeme);

	while (look == COMMA) { move();
		temp = lex.token; match(ID);
		// --- add to symbol table ---
		table_list.save(Tag::VAR_TYPE, temp.lexeme);
	}
	match(SEMICOLON);
}

void Parser::proc()
{
	Token temp; int paraN = 0;
	
	match(PROCEDURE); temp = lex.token; match(ID); match(LPAR);
	table_list.enter_proc(temp.lexeme);	// new symbol table
	
	if (look == ID) { move(); paraN++;
		while (look == COMMA) { move();
			match(ID); paraN++;
		}
	}
	match(RPAR); match(SEMICOLON);
	
	block();
	while (look == SEMICOLON) { move();
		proc();
	}

	void* next_pos = table_list.cur_proc();
	table_list.leave_proc();	// goback symbol table
	// --- add to symbol ---
	table_list.save(Tag::PROC_TYPE, temp.lexeme, paraN, next_pos);
	// table_list.cur_proc()->put(Tag::PROC_TYPE, temp.lexeme, paraN, next_pos);
}

void Parser::body()
{
	match(BEGIN);
	statement();
	while (look == SEMICOLON) { move();
		statement();
	}
	match(END);
}

void Parser::statement()
{
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
	if (is_aop(look)) { move(); }
	term();
	while (is_aop(look)) { move();
		term();
	}	
}

void Parser::term()
{
	factor();
	while (is_mop(look)) { move();
		factor();
	}	
}

void Parser::factor()
{
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
	if (is_lop(look)) move();
	else ;
}

inline void Parser::aop()
{
	if (is_aop(look)) move();
	else ;
}

inline void Parser::mop()
{
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

inline bool Parser::is_aop(Tag t)
{
	return t == ADD || t == SUB;
}

inline bool Parser::is_mop(Tag t)
{
	return t == MUL || t == DIV;
}
