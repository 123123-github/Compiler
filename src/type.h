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
		return 0;
	}
	int proc_level() {
		return 0;
	}
	// const info
	int const_value() {

		return 0;
	}
	// var info
	int var_level() {
		return 0;
	}
	int var_offset() {
		return 0;
	}
	// par info
	int par_level() {
		return 0;
	}
	int par_offset() {
		return 0;
	}
};