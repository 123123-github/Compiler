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
};