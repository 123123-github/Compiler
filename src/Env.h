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

public:
	Env * prev;
	string proc_name;
	map<string, Type> table;	// main table
	int offset_used;
	int level;
public:
	// put symbol in this table
	bool put(Tag t, const string& name, int x, void* table_pos);
	// find symbol by name
	bool get();
};

inline Env::Env()
{
	prev = NULL;
	table.clear();
	offset_used = 0;
}

inline Env::~Env()
{
}

inline bool Env::put(Tag t, const string& name, int x = 0, void* table_pos = NULL)
{
	if (t == Tag::CONST_TYPE) {
		table[name] = Type(CONST_TYPE, 1, 0, x, NULL);
	}
	else if (t == Tag::VAR_TYPE) {
		table[name] = Type(VAR_TYPE, 1, offset_used, 0, NULL);
		offset_used++;
	}
	else if (t == Tag::PROC_TYPE)
	{
		table[name] = Type(PROC_TYPE, x, 0, 0, table_pos);
	}

	return false;
}

inline bool Env::get()
{
	
	return false;
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
	void enter_proc(string proc_name);
	void leave_proc();
	Env* cur_proc();
	bool find(string id);
};

inline EnvList::EnvList()
{
	cur_env = NULL;
}

inline EnvList::~EnvList()
{

}

inline void EnvList::enter_proc(string proc_name)
{
	Env* nextv = new Env;
	nextv->prev = cur_env;
	nextv->proc_name = proc_name;
	nextv->level = cur_env? (cur_env->level + 1): 0;
	cur_env = nextv;
}

inline void EnvList::leave_proc()
{
	cur_env = cur_env->prev;
}

inline Env * EnvList::cur_proc()
{
	return cur_env;
}

inline bool EnvList::find(string id)
{
	return false;
}
