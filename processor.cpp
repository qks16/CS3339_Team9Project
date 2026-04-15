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