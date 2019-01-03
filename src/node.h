#pragma once

#include <vector>
#include <fstream>
#include <assert.h>
using namespace std;

struct Code {
	string op; int L, A;
	Code(string op = "", int L = 0, int A = 0) :op(op), L(L), A(A) {};
};

class Node
{
public:
	Node();
	~Node();
private:
	int nextquad;
	vector<Code> code;
public:
	void emit(const string& op, int L, int A);
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
	nextquad = 0;
	code.clear();
}

inline Node::~Node()
{

}


inline void Node::emit(const string& op, int L, int A)
{
	code.push_back(Code(op, L, A));
	nextquad++;
}

inline void Node::output()
{
	int line = code.size();
	ofstream outfile;
	outfile.open("code.txt");
	assert(outfile.fail());
	for (int i = 0; i < line; i++) {
		outfile << code[i].op << ' ' << code[i].L << code[i].A << '\n';
	}
	outfile.close();
}

inline int Node::get_nextquad()
{
	return nextquad;
}

inline void Node::gen(Gen instr)
{
	// RET WRT NEG
	switch (instr)
	{
	case Gen::WRT:
		emit("OPR", 0, (int)OPR::WRT);
		break;
	case Gen::RET:
		emit("OPR", 0, (int)OPR::RET);
		break;
	case Gen::NEG:
		emit("OPR", 0, (int)OPR::NEG);
		break;
	default:
		break;
	}
}

inline void Node::gen(Gen instr, int x)
{
	switch (instr)
	{
	case Gen::LIT:
		emit("LIT", 0, x);
		break;
	case Gen::INT:
		emit("INT", 0, x);
		break;
	case Gen::PAR:
		emit("PAR", 0, x);
		break;
	case Gen::JMP:
		emit("JMP", 0, x);
		break;
	case Gen::JPC:
		emit("JPC", 0, x);
		break;
	default:
		break;
	}
}

inline void Node::gen(Gen instr, Tag op)
{
	switch (op)
	{
	case Tag::ODD:
		emit("OPR", 0, (int)OPR::ODD);
		break;
	case Tag::EQ:
		emit("OPR", 0, (int)OPR::EQ);
		break;
	case Tag::NE:
		emit("OPR", 0, (int)OPR::NE);
		break;
	case Tag::LESS:
		emit("OPR", 0, (int)OPR::LT);
		break;
	case Tag::LE:
		emit("OPR", 0, (int)OPR::LE);
		break;
	case Tag::GREATER:
		emit("OPR", 0, (int)OPR::GT);
		break;
	case Tag::GE:
		emit("OPR", 0, (int)OPR::GE);
		break;
	case Tag::ADD:
		emit("OPR", 0, (int)OPR::ADD);
		break;
	case Tag::SUB:
		emit("OPR", 0, (int)OPR::SUB);
		break;
	case Tag::MUL:
		emit("OPR", 0, (int)OPR::MUL);
		break;
	case Tag::DIV:
		emit("OPR", 0, (int)OPR::DIV);
		break;
	default:
		break;
	}
}

inline void Node::gen(Gen instr, Type name_info)
{
	switch (instr)
	{
	case Gen::CAL:
		emit("CAL", name_info.proc_level(), name_info.proc_pos());
		break;
	case Gen::STO:
		
		//emit("STO", name_info.proc_level(), name_info.proc_pos());
		
		break;
	case Gen::RED:
		//emit("LOD", name_info.proc_level(), );
		//emit("STO", name_info.proc_level(), name_info.proc_pos());
		break;
	default:
		break;
	}


}

inline void Node::backpatch(int code_pos, int jmp_pos)
{
	code[code_pos].A = jmp_pos;
}
