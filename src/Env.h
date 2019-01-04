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
	int total_para_num;
public:
	bool put(Tag t, const string& name, int table_info = 0, int code_pos_info = 0, void* table_pos = NULL);
	bool get(const string& name, Type& name_info);
};

inline Env::Env()
{
	prev = NULL;
	table.clear();
	offset_used = 0;
	level = 0;
	total_para_num = 0;
}

inline Env::~Env()
{
	if (prev) delete prev;
}

// put symbol in this table
inline bool Env::put(Tag t, const string& name, int table_info, int code_pos_info, void* table_pos)
{
	// deal error...here ->
	// if DEFINE then error()
	// deal error...here <-

	if (t == Tag::CONST_TYPE) {
		table[name] = Type(Tag::CONST_TYPE, table_info, 0, NULL);
		// table _info is the value of CONST_TYPE
	}
	else if (t == Tag::VAR_TYPE) {
		table[name] = Type(Tag::VAR_TYPE, offset_used, 0, NULL);
		offset_used++;
	}
	else if (t == Tag::PROC_TYPE) {
		table[name] = Type(Tag::PROC_TYPE, table_info, code_pos_info, table_pos);
	}
	else if (t == Tag::PARA_TYPE) {
		table[name] = Type(Tag::PARA_TYPE, table_info, code_pos_info, table_pos);
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
	bool find(const string& name, Type& type_info, int& cur_level, int& para_num);
	bool save(Tag t, const string& name, int table_info = 0, int code_pos_info = 0, void* table_pos = NULL);
	void set_total_para_num(int para_num);
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

inline bool EnvList::find(const string& name, Type& type_info, int& cur_level, int& para_num)
{
	for (Env* p = cur_env; p; p = p->prev) {
		if (p->get(name, type_info)) {
			cur_level = p->level;
			para_num = p->total_para_num;
			return true;
		}
	}
	return false;
}

inline bool EnvList::save(Tag t, const string& name, int table_info, int code_pos_info, void* table_pos)
{
	if (cur_env->put(t, name, table_info, code_pos_info, table_pos)) return true;
	else return false;
}

inline void EnvList::set_total_para_num(int para_num)
{
	cur_env->total_para_num = para_num;
}