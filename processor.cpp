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
    

    };

    class ALU {

    };

    class Data_Memory {

    };

    class Control_Unit {
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
    
    };

}