#pragma once

#include <vector>
#include <fstream>
#include <assert.h>
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
	case WRT:
		emit("OPR", 0, OPR::WRT);
		break;
	case RET:
		emit("OPR", 0, OPR::RET);
		break;
	case NEG:
		emit("OPR", 0, OPR::NEG);
		break;
	default:
		break;
	}
}

inline void Node::gen(Gen instr, int x)
{
	switch (instr)
	{
	case LIT:
		emit("LIT", 0, x);
		break;
	case INT:
		emit("INT", 0, x);
		break;
	case PAR:
		emit("PAR", 0, x);
		break;
	case JMP:
		emit("JMP", 0, x);
		break;
	case JPC:
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
	case ODD:
		emit("OPR", 0, OPR::ODD);
		break;
	case EQ:
		emit("OPR", 0, OPR::EQ);
		break;
	case NE:
		emit("OPR", 0, OPR::NE);
		break;
	case LESS:
		emit("OPR", 0, OPR::LT);
		break;
	case LE:
		emit("OPR", 0, OPR::LE);
		break;
	case GREATER:
		emit("OPR", 0, OPR::GT);
		break;
	case GE:
		emit("OPR", 0, OPR::GE);
		break;
	case ADD:
		emit("OPR", 0, OPR::ADD);
		break;
	case SUB:
		emit("OPR", 0, OPR::SUB);
		break;
	case MUL:
		emit("OPR", 0, OPR::MUL);
		break;
	case DIV:
		emit("OPR", 0, OPR::DIV);
		break;
	default:
		break;
	}
}

inline void Node::gen(Gen instr, Type name_info)
{
	switch (instr)
	{
	case CAL:
		emit("CAL", name_info.proc_level(), name_info.proc_pos());
		break;
	case STO:
		
		//emit("STO", name_info.proc_level(), name_info.proc_pos());
		
		break;
	case RED:
		//emit("LOD", name_info.proc_level(), );
		//emit("STO", name_info.proc_level(), name_info.proc_pos());
		break;
	default:
		break;
	}


}
