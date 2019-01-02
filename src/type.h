#pragma once

#include "tag.h"

class Type
{
public:
	Type();
	Type(Tag t, int width, int offset, int value, void* nextv);
	~Type();

private:
	Tag type;	// CONST, VAR, PARA, PROC
	int width;
	int offset;
	int value;
	void* nextv;
};

Type::Type()
{
}

inline Type::Type(Tag tag, int width, int offset, int value, void * nextv)
{
	this->width = width;
	this->offset = offset;
	this->value = value;
	this->nextv = nextv;
}

Type::~Type()
{
}