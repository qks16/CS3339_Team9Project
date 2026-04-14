#include <iostream>
#include <string>

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
            Instruction_Memory(int size = 256) :  pc(0), SIZE(size) {
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

            void setInstruction(int address, string instruction) {
                if (address < 0 || address >= SIZE) {
                    throw out_of_range("Address out of range");
                }
                instruction_set[address] = instruction;
            }

    };

    class Register_File {
    

    };

    class ALU {

    };

    class Data_Memory {

    };

    class Control_Unit {
    
    };

}


/*test instruction set
"ADD R1, R2, R3",
"SUB R4, R5, R6",
"AND R7, R8, R9",
"OR R10, R11, R12",
"XOR R13, R14, R15",
"NOT R16, R17",
"SHL R18, R19, 2",
"SHR R20, R21, 2",
"JMP 100",
"BEQ R22, R23, 50"*/
