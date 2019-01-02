#pragma once
#include "tag.h"
#include <string>
using namespace std;

class Token
{
public:
	Token(Tag t = Tag::ERROR, int v = 0, string s = "") : tag(t), value(v), lexeme(s) {};
	~Token() {};
public:
	Tag tag;
	int value;
	string lexeme;
};
