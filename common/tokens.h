#pragma once

enum Tag{
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
};
typedef Tag Token;