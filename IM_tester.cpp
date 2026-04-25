#include <iostream>
#include <string>
#include <fstream>

#include "processor.cpp"

using namespace std;
using namespace processor;

// simple test case
// "ADD, R1, R2, R3"
// "SUB, R4, R5, R6"

int main(int argc, char* argv[]) {

    int instruction_memory_size = atoi(argv[1]);
    Instruction_Memory im(instruction_memory_size); // create an instruction memory with default size 2
    
    string R_TypeOperand[7] = {"ADD", "SUB", "MUL", "AND", "OR", "SLL", "SRL"};
    string I_TypeOperand[5] = {"ADDI", "LW", "SW", "BEQ"};
    string J_TypeOperand[1] = {"J"};

    ifstream infile(argv[2]);
    
    im.loadInstructions(infile);

        while (im.getPC() < instruction_memory_size) {
            cout << "Executing instruction at PC: " << im.getPC() << " - " << im.getInstruction() << endl;

            //find operand, source1, source2, and destination registers by parsing the instruction string

            string operand = im.getInstruction().substr(0, im.getInstruction().find(",")); // get the operand (e.g. "ADD")
            string rest = im.getInstruction().substr(im.getInstruction().find(",") + 1); // get the rest of the instruction (e.g. " R1, R2, R3")
            string source1 = rest.substr(0, rest.find(",")); // get the first source register (e.g. " R1")
            rest = rest.substr(rest.find(",") + 1); // get the rest of the instruction (e.g. " R2, R3")
            string source2 = rest.substr(0, rest.find(",")); // get the second source register (e.g. " R2")
            string destination = rest.substr(rest.find(",") + 1); // get the destination register (e.g. " R3")

            //display the parsed instruction components
            cout << "Operand: " << operand << endl;
            cout << "Source 1: " << source1 << endl;
            cout << "Source 2: " << source2 << endl;
            cout << "Destination: " << destination << endl;

            if(im.getPC() == instruction_memory_size - 1) {
                cout << "end of program." << endl;
                break;
            }

            im.setPC(im.getPC() + 1); // move to next instruction
        }


    return 0;

    /*MIPS simulation instruction set
    R-type instructions:
    "ADD, R1, R2, R3",
    "SUB, R4, R5, R6",
    "MUL, R1, R2, R3",
    "AND, R7, R8, R9",
    "OR, R10, R11, R12",
    "SLL, R18, R19, 2",
    "SRL, R20, R21, 2",
    I-type instructions:
    "ADDI, R1, R2, 10",
    "LW, R1, 100(R2)",
    "SW, R1, 100(R2)",
    "BEQ, R1, R2, 50",
    J-type instructions:
    "J, 100",
    misc instructions:
    "NOP",*/
}