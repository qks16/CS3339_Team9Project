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
    //printStatement();

    int instruction_memory_size = atoi(argv[1]);
    Instruction_Memory im(instruction_memory_size); // create an instruction memory with default size 2
    
    ifstream infile(argv[2]);

    //simple loader for instructions from a file, each line in the file is an instruction
    for(int address = 0; !infile.eof(); address++) {
        //check if we have reached the end of the instruction memory
        if(address == instruction_memory_size - 1) {
                break;
            }
        string instruction;
        getline(infile, instruction); // read instruction from file
        
        im.setInstruction(address, instruction); // set instruction at current address
    }
        
        cout << "PC: " << im.getPC() << endl; // should print 0
        cout << "Instruction at PC: " << im.getInstruction() << endl; // should print "ADD R1, R2, R3"
    
        im.setPC(1); // set PC to 1

        cout << "PC: " << im.getPC() << endl; // should print 1
        cout << "Instruction at PC: " << im.getInstruction() << endl; // should print "SUB R4, R5, R6"

        im.setPC(0); // reset PC to 0

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

    /*test instruction set
    "ADD, R1, R2, R3",+
    "SUB, R4, R5, R6",+
    "AND, R7, R8, R9",+
    "OR, R10, R11, R12",+
    "SLL, R18, R19, 2",+
    "SRR, R20, R21, 2",+
    "JMP, 100",
    "BEQ, R22, R23, 50"*/
}