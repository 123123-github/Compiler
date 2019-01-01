#pragma once
#include <map>
#include <string>
using namespace std;

class Env
{
public:
	Env();
	~Env();

private:
	Env * prev;
	string block_name;
	//map<string, > table;

};

Env::Env()
{
}

Env::~Env()
{
}