#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <assert.h>
using namespace std;


// code structure & CODE MEM
string op; int L, A;
struct Code {
	string op; int L, A; 
	Code(string op = "", int L = 0, int A = 0) :op(op), L(L), A(A) {};
};
vector<Code> code;

// STACK MEM
const int maxn = 805;
int stack[maxn];

// REG decls
int IP, SP, PC, TP;

// other...
bool finish = false;
int paraN, display, base, value;
enum Tag {
	// opr type
	RET, NOT, ADD, SUB, MUL, DIV, ODD, NUL,
	EQ, NE, LT, GE, GT, LE, WRT, EDL, RED,
	// instr type
	INT, CAL, LIT, LOD, STO, JMP, JPC, OPR, PAR,
};
map<string, int> h;


void load_code()
{
	ifstream infile;
	infile.open("vm_code.txt");
	assert(infile.is_open());

	while (!infile.eof()) {
		infile >> op >> L >> A;
		code.push_back(Code(op, L, A));
	}
	infile.close();
}

void init_envi()
{
	// init stack
	memset(stack, -1, sizeof(stack));
	stack[2] = stack[3] = 0;
	// init regs
	TP = -1; IP = PC = SP = 0;
	// instr type
	h["INT"] = INT; h["CAL"] = CAL; h["LIT"] = LIT; h["LOD"] = LOD;
	h["STO"] = STO; h["JMP"] = JMP; h["JPC"] = JPC; h["OPR"] = OPR;
	h["PAR"] = PAR;
	// assitance
	paraN = 0;
	display = SP + 4;
	stack[display] = 0;
}

void par()
{
	paraN = A;
	stack[TP + 4] = paraN;				// save PARA NUM
	// pass value
	for (int i = 1; i <= A; i++) {
		stack[TP + 4 + i] = stack[TP - A + i];
	}
	
	//paraN++;
	//stack[TP+4+paraN] = A;		// pass value
	//int base = stack[display + L];
	//int value = stack[base + A];
	//stack[TP + 4 + paraN] = value;
}

void cal()
{
	stack[TP + 1] = SP;			// save old SP
	stack[TP + 2] = PC;			// save RET ADDRESS
	stack[TP + 3] = display;	// save GLOBAL DISPLAY_BASE
	// stack[TP + 4] = paraN;		// save PARA NUM

	// copy display table
	int pre_display = display;		
	display = TP + 5 + stack[TP + 4];
	for (int i = 0; i<L; i++) {
		stack[display + i] = stack[pre_display + i];
	}
	stack[display + L] = TP + 1;	// init DISPLAY TABLE

	PC = A;							// jump to func...
}

inline void Int() {
	SP = TP + 1;		// new SP
	TP = TP + A;		// new TOP
}

void lod()
{
	// L is the var LEVEL
	base = stack[display + L];
	value = stack[base + A];
	stack[++TP] = value;
}

void sto()
{
	value = stack[TP];
	base = stack[display + L];
	stack[base + A] = value;
	TP--;
}

inline void jmp() { PC = A; }
inline void jpc() { if (!stack[TP]) PC = A; }
inline void lit() { stack[++TP] = A; }

void opr()
{
	switch (A) {
	case RET:	// func return
		display = stack[SP + 2];		// recover DISPLAY
		PC = stack[SP + 1];
		TP = SP - 1; SP = stack[SP];
		if (SP < 0) finish = true;
		break;
	case NOT:
		stack[TP] = -stack[TP];
		break;
	case ADD:
		stack[TP - 1] = stack[TP - 1] + stack[TP]; TP--;
		break;
	case SUB:
		stack[TP - 1] = stack[TP - 1] - stack[TP]; TP--;
		break;
	case MUL:
		stack[TP - 1] = stack[TP - 1] * stack[TP]; TP--;
		break;
	case DIV:
		stack[TP - 1] = stack[TP - 1] / stack[TP]; TP--;
		break;
	case ODD:
		stack[TP] = stack[TP] % 2;
		break;
	case NUL: break; // null 
	case EQ:
		stack[TP - 1] = (stack[TP - 1] == stack[TP]); TP--;
		break;
	case NE:
		stack[TP - 1] = (stack[TP - 1] != stack[TP]); TP--;
		break;
	case LT:
		stack[TP - 1] = (stack[TP - 1] < stack[TP]); TP--;
		break;
	case GE:
		stack[TP - 1] = (stack[TP - 1] >= stack[TP]); TP--;
		break;
	case GT:
		stack[TP - 1] = (stack[TP - 1] > stack[TP]); TP--;
		break;
	case LE:
		stack[TP - 1] = (stack[TP - 1] <= stack[TP]); TP--;
		break;
	case WRT: cout << "output:\t" << stack[TP--] << endl; break;
	case EDL: cout << endl; break;
	case RED: cout << "input:\t"; cin >> stack[++TP]; break;
	default: assert(-1);
	}
}

int main()
{
	load_code();
	init_envi();

	Code cur;
	while (!finish)
	{
		cur = code[IP];
		op = cur.op; L = cur.L; A = cur.A;
		PC = IP + 1;

		switch (h[op]) {
		case CAL: cal(); break;
		case LIT: lit(); break;
		case LOD: lod(); break;
		case STO: sto(); break;
		case JMP: jmp(); break;
		case JPC: jpc(); break;
		case OPR: opr(); break;
		case INT: Int(); break;
		case PAR: par(); break;
		default: assert(-1);
		}

		IP = PC;
	}

	cout << "--- Program Finish! ---\n";
	system("pause");
	return 0;
}

/*
display Table:
| *NULL*
TP->	| VAR
| display-L
| ...
| display-1
display->| display-0
| para...N
| ...
| para...1
| paraN
| global DISPLAY
| ret ADDR
SP->	| old SP
----------------------
*/