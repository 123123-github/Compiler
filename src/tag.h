#pragma once

enum Tag{
	// lexer read
	ERROR = 0,
	PROGRAM, 
	CONST, ASSIGN, VAR, PROCEDURE,
	BEGIN, END,
	IF, THEN, ELSE,
	WHILE, DO,
	CALL, READ, WRITE,
	ODD, 
	LOP, EQ, NE, LESS, LE, GREATER, GE,
	AOP, ADD, SUB,
	MOP, MUL, DIV,
	ID, NUM,
	COMMA, SEMICOLON,		// , ;
	LPAR, RPAR,				// ( )
	OVER,					// file over
	
	// symbol table
	VAR_TYPE = 256,
	CONST_TYPE,
	PROC_TYPE,
	PARA_TYPE,
};

enum Gen
{
	// instr type
	INT, CAL, LIT, LOD, STO, JMP, JPC, OPR, PAR,
	// assistant instr type
	RED, WRT, RET, NEG,
};