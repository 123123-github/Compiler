#pragma once

class Node
{
public:
	Node();
	~Node();
private:
	int nextquad;
public:
	void emit();
	void output();
	int  get_nextquad();
	void gen(Gen instr);
	void gen(Gen instr, int x);
	void gen(Gen instr, Tag op);
	void gen(Gen instr, Type name_info);
	void backpatch(int code_pos, int jmp_pos);
};

inline Node::Node()
{
}

inline Node::~Node()
{
}

