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
	
	if (lex.token.tag != Tag::OVER) {
		get_error();
	}

	node.output();
}

inline void Parser::move()
{
	while ((look = (Tag)lex.nextToken().tag) == Tag::ERROR);
}

inline bool Parser::match(Tag t)
{
	if (look == t) move();
	else 
	{
		get_error();
		if (look == Tag::OVER) return true;
		while ( look != t && !follow[(int)t][(int)look] ) move();
		if (look == t) move();
	}
		
	return true;
}

void Parser::get_error()
{
	fprintf(stderr, "Line:%3d, Col:%3d: syntax error! \n", lex.get_line(), lex.get_col());
	if (lex.token.tag == Tag::OVER) {
		fprintf(stderr, "File END! \n");
		//exit(1);
	}
}

void Parser::prog()
{
	Token temp;
	match(Tag::PROGRAM); temp = lex.token; match(Tag::ID); match(Tag::SEMICOLON);
	
	table_list.enter_proc(temp.lexeme);	// new symbol table
	block();
	table_list.leave_proc();			// goback symbol table
}

void Parser::block()
{
	// --- gen code ---
	int m1 = node.get_nextquad();
	node.gen(Gen::JMP, -1);

	if (look == Tag::CONST)	condecl();
	if (look == Tag::VAR)	vardecl();
	if (look == Tag::PROCEDURE) { proc(); }
	
	// --- gen code ---
	node.backpatch(m1, node.get_nextquad());
	int size = table_list.cur_proc()->offset_used;
	int level = table_list.cur_proc()->level;
	int para_num = table_list.cur_proc()->total_para_num;
	node.gen(Gen::INT, size, level, para_num);

	body();

	// --- gen code ---
	node.gen(Gen::RET);
}

void Parser::condecl()
{
	match(Tag::CONST);
	constexp();
	while (look == Tag::COMMA) { move();
		constexp();
	}
	match(Tag::SEMICOLON);
}

void Parser::constexp()
{
	Token temp1, temp2;
	temp1 = lex.token; match(Tag::ID); match(Tag::ASSIGN); temp2 = lex.token; match(Tag::NUM);
	// --- add to symbol table ---
	table_list.save(Tag::CONST_TYPE, temp1.lexeme, temp2.value);
}

void Parser::vardecl()
{
	Token temp;
	match(Tag::VAR); temp = lex.token; match(Tag::ID);
	// --- add to symbol table ---
	table_list.save(Tag::VAR_TYPE, temp.lexeme);

	while (look == Tag::COMMA) { move();
		temp = lex.token; match(Tag::ID);
		// --- add to symbol table ---
		table_list.save(Tag::VAR_TYPE, temp.lexeme);
	}
	match(Tag::SEMICOLON);
}

void Parser::proc()
{
	Token temp; 
	int paraN = 0, code_pos = node.get_nextquad();
	
	match(Tag::PROCEDURE); temp = lex.token; match(Tag::ID); match(Tag::LPAR);
	table_list.enter_proc(temp.lexeme);	// --- new symbol table ---
	if (look == Tag::ID) { 
		// --- add to symbol ---
		table_list.save(Tag::PARA_TYPE, lex.token.lexeme, paraN);
		move(); paraN++;
		while (look == Tag::COMMA) { move();
			// --- add to symbol ---
			table_list.save(Tag::PARA_TYPE, lex.token.lexeme, paraN);
			match(Tag::ID); paraN++;
		}
		// symbol para_num set
		table_list.set_total_para_num(paraN);
	}
	match(Tag::RPAR); match(Tag::SEMICOLON);
	
	block();

	void* next_pos = table_list.cur_proc();
	table_list.leave_proc();			// --- goback symbol table ---
	// --- add to symbol ---
	table_list.save(Tag::PROC_TYPE, temp.lexeme, paraN, code_pos, next_pos);
	
	while (look == Tag::SEMICOLON) { move();		// --- next proc ---
		proc();
	}
}

void Parser::body()
{
	match(Tag::BEGIN);
	statement();
	while (look == Tag::SEMICOLON) { move();
		statement();
	}
	match(Tag::END);
}

void Parser::statement()
{
	Type type_temp; Token token_temp;
	int m1, m2 = -1, paraN = 0, level, cur_proc_total_para;

	switch (look) {
	case Tag::ID:										// ASSIGN---
		// SYMBOL TABLE - find name
		token_temp = lex.token;
		table_list.find(token_temp.lexeme, type_temp, level, cur_proc_total_para);
		move(); match(Tag::ASSIGN); exp();
		// --- gen code ---
		node.gen(Gen::STO, type_temp, level, cur_proc_total_para);
		break;
	case Tag::IF: move();								// IF---ELSE---
		lexp();
		// --- gen code ---
		m1 = node.get_nextquad();		// JPC
		node.gen(Gen::JPC, -1);
		match(Tag::THEN); statement();
		if (look == Tag::ELSE) { move();
			m2 = node.get_nextquad();
			node.gen(Gen::JMP, -1);		// JMP
			statement();
		}
		if (m2 < 0) {		// IF ---
			node.backpatch(m1, node.get_nextquad());
		}
		else {				// IF --- ELSE ---
			node.backpatch(m1, m2);
			node.backpatch(m2 + 1, node.get_nextquad());
		}
		break;
	case Tag::WHILE: move();							// WHILE---
		// --- gen code ---
		m1 = node.get_nextquad(); 
		lexp();
		m2 = node.get_nextquad();
		node.gen(Gen::JPC, -1);			// JPC
		match(Tag::DO); statement();
		// --- gen code ---
		node.gen(Gen::JMP, m1);			// JMP
		node.backpatch(m2, node.get_nextquad());
		break;
	case Tag::CALL: move();								// CALL---
		// SYMBOL TABLE - find name
		token_temp = lex.token;
		table_list.find(token_temp.lexeme, type_temp, level, cur_proc_total_para);
		match(Tag::ID); match(Tag::LPAR);
		if (is_exp(look)) { 
			exp(); paraN++;
			while (look == Tag::COMMA) {
				move(); exp(); paraN++;
			}
		}
		match(Tag::RPAR);
		// --- gen code ---
		// find error here... paraN == 
		node.gen(Gen::PAR, paraN);
		node.gen(Gen::CAL, type_temp, level + 1, cur_proc_total_para = -1);
		break;
	case Tag::BEGIN:									// BODY---
		body();
		break;
	case Tag::READ: move();								// READ---
		match(Tag::LPAR);
		// SYMBOL TABLE - find name
		token_temp = lex.token;
		table_list.find(token_temp.lexeme, type_temp, level, cur_proc_total_para);
		match(Tag::ID);
		// --- gen code ---
		node.gen(Gen::RED, type_temp, level, cur_proc_total_para);
		while (look == Tag::COMMA) { move();
			// SYMBOL TABLE - find name
			token_temp = lex.token;
			table_list.find(token_temp.lexeme, type_temp, level, cur_proc_total_para);
			// --- gen code ---
			node.gen(Gen::RED, type_temp, level, cur_proc_total_para);
			match(Tag::ID);
		}
		match(Tag::RPAR);
		break;
	case Tag::WRITE: move();								// WRITE---
		match(Tag::LPAR);
		exp();
		// --- gen code ---
		node.gen(Gen::WRT);
		while (look == Tag::COMMA) { move();
			exp();
			// --- gen code ---
			node.gen(Gen::WRT);
		}
		match(Tag::RPAR);
		break;
	default:
		;
	}
			
}

void Parser::lexp()
{
	Tag op;

	switch (look) {
	case Tag::ODD: move();
		exp();
		node.gen(Gen::OPR, Tag::ODD);
		break;
	case Tag::ADD:
	case Tag::SUB:
	case Tag::ID:
	case Tag::NUM:
	case Tag::LPAR:
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
	int level, cur_proc_total_para;
	Token token_temp; Type type_temp;
	switch (look) {
	case Tag::ID:
		// SYMBOL TABLE - find name
		token_temp = lex.token;
		table_list.find(token_temp.lexeme, type_temp, level, cur_proc_total_para);
		move();
		// --- gen code ---
		node.gen(Gen::LOD, type_temp, level, cur_proc_total_para);
		break;
	case Tag::NUM:
		token_temp = lex.token;
		move();
		// --- gen code ---
		node.gen(Gen::LIT, token_temp.value);
		break;
	case Tag::LPAR: move();
		exp(); match(Tag::RPAR);
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
	return Tag::EQ <= t && t<= Tag::GE;
}

bool Parser::is_exp(Tag t)
{
	return is_aop(t) || t == Tag::ID || t == Tag::NUM || t == Tag::LPAR;
}

inline bool Parser::is_aop(Tag t)
{
	return t == Tag::ADD || t == Tag::SUB;
}

inline bool Parser::is_mop(Tag t)
{
	return t == Tag::MUL || t == Tag::DIV;
}