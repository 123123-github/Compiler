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
	// --- gen code ---
	int m1 = node.get_nextquad();
	node.gen(Gen::JMP, -1);

	if (look == CONST)	condecl();
	if (look == VAR)	vardecl();
	if (look == PROCEDURE) { proc(); }
	
	// --- gen code ---
	node.backpatch(m1, node.get_nextquad());
	int size = table_list.cur_proc()->offset_used;
	node.gen(Gen::INT, size);

	body();

	// --- gen code ---
	node.gen(Gen::RET);
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
	Type type_temp; Token token_temp;
	int m1, m2, paraN;

	switch (look) {
	case ID:										// ASSIGN---
		// SYMBOL TABLE - find name
		token_temp = lex.token;
		table_list.find(token_temp.lexeme, type_temp);
		move(); match(ASSIGN); exp();
		// --- gen code ---
		node.gen(Gen::LOD, type_temp);
		break;
	case IF: move();								// IF---ELSE---
		lexp();
		// --- gen code ---
		m1 = node.get_nextquad();		// JPC
		node.gen(Gen::JPC, -1);
		match(THEN); statement();
		if (look == ELSE) { move();
			m2 = node.get_nextquad();
			node.gen(Gen::JMP, -1);		// JMP
			statement();
		}
		node.backpatch(m1, m2);
		node.backpatch(m2 + 1, node.get_nextquad());
		break;
	case WHILE: move();								// WHILE---
		m1 = node.get_nextquad(); lexp(); 
		// --- gen code ---
		node.gen(Gen::JPC, -1);			// JPC
		match(DO); statement();
		m2 = node.get_nextquad();
		node.backpatch(m1, m2);
		// --- gen code ---
		node.gen(Gen::JMP, m1);			// JMP
		break;
	case CALL: move();								// CALL---
		// SYMBOL TABLE - find name
		token_temp = lex.token;
		table_list.find(token_temp.lexeme, type_temp);
		match(ID); match(LPAR);
		if (is_exp(look)) { 
			exp(); paraN = 1;
			while (look == COMMA) { 
				move(); exp(); paraN++;
			}
		}
		match(RPAR);
		// --- gen code ---
		node.gen(Gen::PAR, paraN);
		node.gen(Gen::CAL, type_temp);
		break;
	case BEGIN:										// BODY---
		body();
		break;
	case READ: move();								// READ---
		match(LPAR);
		// SYMBOL TABLE - find name
		token_temp = lex.token;
		table_list.find(token_temp.lexeme, type_temp);
		match(ID);
		// --- gen code ---
		node.gen(Gen::RED, type_temp);
		while (look == COMMA) { move();
			// SYMBOL TABLE - find name
			token_temp = lex.token;
			table_list.find(token_temp.lexeme, type_temp);
			// --- gen code ---
			node.gen(Gen::RED, type_temp);
			match(ID);
		}
		match(RPAR);
		break;
	case WRITE: move();								// WRITE---
		match(LPAR); 
		exp();
		// --- gen code ---
		node.gen(Gen::WRT);
		while (look == COMMA) { move();
			exp();
			// --- gen code ---
			node.gen(Gen::WRT);
		}
		match(RPAR);
		break;
	default:
		;
	}
			
}

void Parser::lexp()
{
	Tag op;

	switch (look) {
	case ODD: move();
		exp();
		node.gen(Gen::OPR, Tag::ODD);
		break;
	case ADD:
	case SUB:
	case ID:
	case NUM:
	case LPAR:
		exp(); op = look; lop(); exp();
		// --- gen code ---
		node.gen(Gen::OPR, op);
		break;
	default:
		;
	}	
}

void Parser::exp()
{
	Tag op = Tag::ERROR;

	if (is_aop(look)) { op = look; move(); }
	term();
	// --- gen code ---
	if (op == Tag::SUB) { node.gen(Gen::NEG); }

	while (is_aop(look)) { 
		op = look;
		move(); term();
		// --- gen code ---
		node.gen(Gen::OPR, op);
	}	
}

void Parser::term()
{
	factor();
	while (is_mop(look)) { 
		Tag op = look;
		move(); factor();
		// --- gen code ---
		node.gen(Gen::OPR, op);
	}	
}

void Parser::factor()
{
	Token token_temp; Type type_temp;
	switch (look) {
	case ID: 
		// SYMBOL TABLE - find name
		token_temp = lex.token;
		table_list.find(token_temp.lexeme, type_temp);
		move();
		// --- gen code ---
		node.gen(Gen::LOD, type_temp);
		break;
	case NUM: 
		token_temp = lex.token;
		move();
		// --- gen code ---
		node.gen(Gen::LIT, token_temp.value);
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
