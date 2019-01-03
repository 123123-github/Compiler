#pragma once

#include "tag.h"

class Type
{
public:
	Type() {};
	~Type() {};
	Type(Tag t, int width, int offset, int value, void* nextv) : type(t), width(width), offset(offset), value(value), nextv(nextv) {}

private:
	Tag type;	// CONST, VAR, PARA, PROC
	int width;
	int offset;
	int value;
	void* nextv;
public:
	// proc info
	int proc_pos() {
	
	}
	int proc_level() {
	
	}
	// const info
	int const_value() {

	
	}
	// var info
	int var_level() {

	}
	int var_offset() {

	}
	// par info
	int par_level() {

	}
	int par_offset() {

	}
};