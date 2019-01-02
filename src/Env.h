#pragma once
#include <map>
#include <string>
#include "type.h"
using namespace std;

class Env
{
public:
	Env();
	~Env();

private:
	Env * prev;
	string block_name;
	map<string, Type> table;	// main table
};

Env::Env()
{
	table.clear();
}

Env::~Env()
{
}

//-------------------------------
class EnvList
{
public:
	EnvList();
	~EnvList();

private:
	Env * cur_env;

public:
	void enter_block();
	void leave_block();
	Env* cur_block();
};

EnvList::EnvList()
{
}

EnvList::~EnvList()
{
}

inline void EnvList::enter_block()
{
}

inline void EnvList::leave_block()
{
}

inline Env * EnvList::cur_block()
{
	return NULL;
}
