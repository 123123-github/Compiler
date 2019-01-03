#pragma once

class Node
{
public:
	Node();
	~Node();
private:
	int nextquad;
public:
	void gen(Tag t);				// a + b
	void gen(Tag t, int opnd_num);	// -a
	void gen(Type opnd);			// lod CONST VAR PARA PROC
	void gen(Type oped, Tag t);		// assign
	void gen(int imm);				// 5
	int get_nextquad();
	void backpatch(int code_pos, int jmp_pos);
};

inline Node::Node()
{
}

inline Node::~Node()
{
}

