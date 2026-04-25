#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <unordered_map>
#include <iomanip>

using namespace std;

namespace processor {
    class Instruction_Memory {
        private:
            int pc; // program counter
            int SIZE; // size of instruction memory

            // string instruction_set[256];

            string* instruction_set; // dynamic array for instruction set

        public:

            //instruction memory constructor, used to initialize the instruction memory with a given size
            //if size is not provided, default to 256
            Instruction_Memory(int size) :  pc(0), SIZE(size) {
                instruction_set = new string[size];
            }

            // destructor to free the dynamically allocated instruction set memory
            ~Instruction_Memory() {
                delete[] instruction_set;
            }

            int getPC() {
                return pc;
            }

            void setPC(int newPC) {
                pc = newPC; // change in PC for next instruction

                if (pc < 0 || pc >= SIZE) {
                    throw out_of_range("PC out of range");
                }
            }

            string getInstruction() {
                return instruction_set[pc];
            }

            //simple loader for instructions from a file, each line in the file is an instruction
            void loadInstructions(ifstream& infile) {
                for(int address = 0; !infile.eof(); address++) {
                    string instruction;
                    getline(infile, instruction); // read instruction from file

                    if (address < 0 || address >= SIZE) {
                    throw out_of_range("Address out of range");
                    }
        
                    instruction_set[address] = instruction; // set instruction at current address
                }
            }

    };

    class Register_File {
    private:
        array<int,32> regs{};
    public:
        Register_File() { regs.fill(0); }

        int read(int r) const {
            if (r < 0 || r > 31) throw out_of_range("Register read OOB");
            return regs[r];
        }

        void write(int r, int v) {
            if (r < 0 || r > 31) throw out_of_range("Register write OOB");
            if (r != 0) regs[r] = v;
        }

        void print() const {
            cout << "┌─────────────────── Register File ───────────────────┐\n";
            for (int i = 0; i < 32; ++i) {
                cout << "│ R" << setw(2) << setfill('0') << i << setfill(' ')
                     << " = " << setw(10) << regs[i] << "  ";
                if (i % 4 == 3) cout << "│\n";
            }
            cout << "└──────────────────────────────────────────────────────┘\n";
        }
    };

    class ALU {
    public:
        int execute(const string& op, int a, int b, int shamt = 0) const {
            if (op=="ADD")  return a + b;
            if (op=="SUB")  return a - b;
            if (op=="MUL")  return a * b;
            if (op=="AND")  return a & b;
            if (op=="OR")   return a | b;
            if (op=="SLL")  return b << shamt;
            if (op=="SRL")  return (int)((unsigned int)b >> shamt);
            throw runtime_error("Unknown ALU op: " + op);
        }
    };

    class Data_Memory {
    private:
        unordered_map<int,int> mem;
    public:
        int  loadWord(int addr)           const { auto it=mem.find(addr); return it==mem.end()?0:it->second; }
        void storeWord(int addr, int val)       { mem[addr]=val; }

        void print() const {
            cout << "┌──────────────────── Data Memory ────────────────────┐\n";
            if (mem.empty()) {
                cout << "│  (no data written)                                  │\n";
            } else {
                for (const auto& [a,v] : mem)
                    cout << "│  Mem[" << setw(5) << a << "] = " << setw(10) << v << "                            │\n";
            }
            cout << "└──────────────────────────────────────────────────────┘\n";
        }
    };

    class Control_Unit {
<<<<<<< HEAD
        private:
            // control signals
            bool RegDst; // register destination
            bool ALUSrc; // ALU source
            bool MemtoReg; // memory to register
            bool RegWrite; // register write
            bool MemRead; // memory read
            bool MemWrite; // memory write
            bool Branch; // branch
            bool ALUOp1; // ALU operation 1
            bool ALUOp0; // ALU operation 0

            //string mathOpr[5] = {"ADD", "SUB", "MUL", "AND", "OR"};

        public:
            // control unit constructor, used to initialize the control signals based on the instruction type
            Control_Unit(string operand) {

                if (operand == "ADD" || operand == "SUB" || operand == "MUL" || operand == "AND" || operand == "OR") {
                    RegDst = true;
                    ALUSrc = false;
                    MemtoReg = false;
                    RegWrite = true;
                    MemRead = false;
                    MemWrite = false;
                    Branch = false;
                    ALUOp1 = true;
                    ALUOp0 = false;
                } else if (operand == "SLL" || operand == "SRL") {
                    RegDst = true;
                    ALUSrc = true;
                    MemtoReg = false;
                    RegWrite = true;
                    MemRead = false;
                    MemWrite = false;
                    Branch = false;
                    ALUOp1 = true;
                    ALUOp0 = true;
                } else {
                    // default control signals for unsupported instructions
                    RegDst = false;
                    ALUSrc = false;
                    MemtoReg = false;
                    RegWrite = false;
                    MemRead = false;
                    MemWrite = false;
                    Branch = false;
                    ALUOp1 = false;
                    ALUOp0 = false;
                }
            }

            bool isRegDst() {
                return RegDst;
            }

            bool isALUSrc() {
                return ALUSrc;
            }

            bool isMemtoReg() {
                return MemtoReg;
            }

            bool isRegWrite() {
                return RegWrite;
            }

            bool isMemRead() {
                return MemRead;
            }

            bool isMemWrite() {
                return MemWrite;
            }

            bool isBranch() {
                return Branch;
            }

            bool isALUOp1() {
                return ALUOp1;
            }

            bool isALUOp0() {
                return ALUOp0;
            }
    
=======
    public:
        ControlSignals decode(Opcode op) const {
            ControlSignals c;
            switch (op) {
                case Opcode::ADD: case Opcode::SUB: case Opcode::AND:
                case Opcode::OR:  case Opcode::MUL:
                    c.RegDst=1; c.RegWrite=1; c.ALUOp=opcodeToString(op); break;
                case Opcode::SLL: case Opcode::SRL:
                    c.RegDst=1; c.RegWrite=1; c.ALUOp=opcodeToString(op); break;
                case Opcode::ADDI:
                    c.ALUSrc=1; c.RegWrite=1; c.ALUOp="ADD"; break;
                case Opcode::LW:
                    c.ALUSrc=1; c.MemtoReg=1; c.RegWrite=1; c.MemRead=1; c.ALUOp="ADD"; break;
                case Opcode::SW:
                    c.ALUSrc=1; c.MemWrite=1; c.ALUOp="ADD"; break;
                case Opcode::BEQ:
                    c.Branch=1; c.ALUOp="SUB"; break;
                case Opcode::J:
                    c.Jump=1; break;
                default: break;
            }
            return c;
        }
>>>>>>> main
    };

}
