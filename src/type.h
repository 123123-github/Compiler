#pragma once

#include "tag.h"

class Type
{
public:
	Type() {};
	~Type() {};
	Type(Tag t, int table_info, int code_pos_info, void* nextv) : 
		type(t), table_info(table_info), code_pos_info(code_pos_info), nextv(nextv) {}

private:
	Tag type;	// CONST, VAR, PARA, PROC...TYPE
	int table_info;
	int code_pos_info;
	void* nextv;
public:
	Tag get_type() {
		return type;
	}
	int get_table_info() {
		return table_info;
	}
	int get_code_pos_info() {
		return code_pos_info;
	}
};