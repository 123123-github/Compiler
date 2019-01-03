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
	bool put(Tag t, const string& name, int x = 0, void* table_pos = NULL);
	bool get(const string& name, Type& name_info);
};

inline Env::Env()
{
	prev = NULL;
	table.clear();
	offset_used = 0;
}

inline Env::~Env()
{
	if (prev) delete prev;
}

// put symbol in this table
inline bool Env::put(Tag t, const string& name, int x, void* table_pos)
{
	if (t == Tag::CONST_TYPE) {
		table[name] = Type(CONST_TYPE, 1, 0, x, NULL);
		// x is value
	}
	else if (t == Tag::VAR_TYPE) {
		table[name] = Type(VAR_TYPE, 1, offset_used, 0, NULL);
		offset_used++;
		// x is offset_pos
	}
	else if (t == Tag::PROC_TYPE)
	{
		table[name] = Type(PROC_TYPE, x, 0, 0, table_pos);
		// x is paraN
	}

	return false;
}

// find symbol by name
inline bool Env::get(const string& name, Type& name_info)
{
	if (table.count(name)) {
		name_info = table[name];
		return true;
	}
	else  return false;
}

//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
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
	bool find(const string& name, Type& type_info);
	bool save(Tag t, const string& name, int x = 0, void* table_pos = NULL);
	Env* cur_proc();
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

inline bool EnvList::find(const string& name, Type& type_info)
{
	for (Env* p = cur_env; p; p = p->prev) {
		if (p->get(name, type_info)) return true;
	}
	return false;
}

inline bool EnvList::save(Tag t, const string & name, int x, void * table_pos)
{
	if (cur_env->put(t, name, x, table_pos)) return true;
	else return false;
}