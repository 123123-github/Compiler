#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <assert.h>
using namespace std;


// code structure & CODE MEM
string op; int L, A;
struct Code { string op; int L, A; };
vector<Code> code;

// STACK MEM
const int maxn = 805;
int stack[maxn];

// REG decls
int IP, SP, PC, TP;

// other...
enum Tag { 
	// opr type
	RET, NOT, ADD, SUB, MUL, DIV, ODD, NUL, 
	EQ , NE , LT , GE , GT , LE , WRT, EDL, RED,
	
	// instr type
	INT, CAL, LIT, LOD, STO, JMP, JPC, OPR,
};
map<string, int> h;

void load_code()
{
	ifstream infile;
	infile.open("code.txt");
	assert(infile.is_open());
	
	while (!infile.eof()) {
		infile >> op >> L >> A;
		code.push_back((Code){op, L, A});
	}
	infile.close();
}

void init_envi()
{
	// instr type
	h["INT"] = INT; h["CAL"] = CAL; h["LIT"] = LIT; h["LOD"] = LOD;
	h["STO"] = STO; h["JMP"] = JMP; h["JPC"] = JPC; h["OPR"] = OPR;
	
	// opr type
	
	
	// init regs
	TP = -1; IP = PC = SP = 0;
	
	
	
}

void cal()
{
	PC = A;		// jump to func...
	
	
}

inline void lit() { stack[++TP] = A; }

void lod()
{
	
}

void sto()
{
	
}

inline void jmp() { PC = A; }

inline void jpc() { if (!stack[top]) PC = A; }

void opr()
{
	switch(A) {
	case RET:
		TP = SP - 1;
		SP = stack[SP]; 
		break;
	case NOT:
		stack[TP] = -stack[TP]; 
		break;
	case ADD:
		stack[TP-1] = stack[TP-1] + stack[TP]; TP--;
		break;
	case SUB:
		stack[TP-1] = stack[TP-1] - stack[TP]; TP--;
		break;
	case MUL:
		stack[TP-1] = stack[TP-1] * stack[TP]; TP--;
		break;
	case DIV:
		stack[TP-1] = stack[TP-1] / stack[TP]; TP--;
		break;
	case ODD:
		stack[TP] = stack[TP] % 2;
		break;
	case NUL: break; // null 
	case EQ :
		stack[TP-1] = (stack[TP-1] == stack[TP]); TP--;
		break;
	case NE :
		stack[TP-1] = (stack[TP-1] != stack[TP]); TP--;
		break;
	case LT :
		stack[TP-1] = (stack[TP-1] < stack[TP]); TP--;
		break;
	case GE :
		stack[TP-1] = (stack[TP-1] >= stack[TP]); TP--;
		break;
	case GT :
		stack[TP-1] = (stack[TP-1] > stack[TP]); TP--;
		break;
	case LE :
		stack[TP-1] = (stack[TP-1] <= stack[TP]); TP--;
		break;
	case WRT: cout << stack[TP--] << endl; break;
	case EDL: cout << endl; break;
	case RED: cin >> stack[++TP]; break;
	default: assert(-1);
	}
}

int main()
{
	load_code();
	init_envi();
	
	Code cur;
	while (true)
	{
		cur = code[IP];
		PC = IP + 1;
		
		switch(h[op]) {
		case CAL: cal(); break;
		case LIT: lit(); break;
		case LOD: lod(); break;
		case STO: sto(); break;
		case JMP: jmp(); break;
		case JPC: jpc(); break;
		case OPR: opr(); break;
		default: assert(-1);
		}
		
		IP = PC;
	}
		
	return 0;
}
