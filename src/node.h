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
	static const int basic_size = 4;
private:
	int nextquad;
	vector<Code> code;
public:
	void emit(const string& op, int L, int A);
	void output();
	int  get_nextquad();
	void gen(Gen instr);
	void gen(Gen instr, int x, int level, int para_num);
	void gen(Gen instr, Tag op);
	void gen(Gen instr, Type name_info, int level, int para_num);
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
	assert(outfile.is_open());
	for (int i = 0; i < line; i++) {
		outfile << code[i].op << ' ' << code[i].L << ' ' <<code[i].A << '\n';
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

inline void Node::gen(Gen instr, int x, int level = 0, int para_num = 0)
{
	switch (instr)
	{
	case Gen::LIT:
		emit("LIT", 0, x);
		break;
	case Gen::INT:
		emit("INT", 0, x + basic_size + level + 1 + para_num);
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

inline void Node::gen(Gen instr, Type name_info, int level, int para_num)
{
	// offset = _offset + basic_size + [(para_num + 1)] + (level + 1)-->display_table_size
	int offset;
	switch (instr)
	{
	case Gen::CAL:
		emit("CAL", level, name_info.get_code_pos_info());
		break;
	case Gen::LOD:
		if (name_info.get_type() == Tag::VAR_TYPE) {
			offset = name_info.get_table_info() + basic_size + (para_num) +(level + 1);
			emit("LOD", level, offset);							// modifi offset here...
		}
		else if (name_info.get_type() == Tag::CONST_TYPE) {
			emit("LIT", 0, name_info.get_table_info());
		}
		else if (name_info.get_type() == Tag::PARA_TYPE) {
			offset = name_info.get_table_info() + basic_size;
			emit("LOD", level, offset);
		}
		break;
	case Gen::STO:
		offset = name_info.get_table_info() + basic_size;
		if (name_info.get_type() != Tag::PARA_TYPE) {
			offset = offset + (para_num) + (level + 1);
		}
		emit("STO", level, offset);			// modifi offset here...
		break;
	case Gen::RED:
		offset = name_info.get_table_info() + basic_size;
		if (name_info.get_type() != Tag::PARA_TYPE) {
			offset = offset + (para_num) + (level + 1);
		}
		// emit("LOD", level, offset);
		emit("OPR", 0, (int)OPR::RED);
		emit("STO", level, offset);
		break;
	default:
		break;
	}
}

inline void Node::backpatch(int code_pos, int jmp_pos)
{
	code[code_pos].A = jmp_pos;
}
