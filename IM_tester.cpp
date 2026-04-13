#include <iostream>

#include "processor.cpp"

using namespace std;
using namespace processor;

int main(int argc, char* argv[]) {
    //printStatement();

    Instruction_Memory im(256); // create an instruction memory with default size 256
        im.setInstruction(0, "ADD R1, R2, R3");
        im.setInstruction(1, "SUB R4, R5, R6");
    
        
        cout << "PC: " << im.getPC() << endl; // should print 0
        cout << "Instruction at PC: " << im.getInstruction() << endl; // should print "ADD R1, R2, R3"
    
        im.setPC(1); // set PC to 1

        cout << "PC: " << im.getPC() << endl; // should print 1
        cout << "Instruction at PC: " << im.getInstruction() << endl; // should print "SUB R4, R5, R6"

        while (im.getPC() < 2) {
            cout << "Executing instruction at PC: " << im.getPC() << " - " << im.getInstruction() << endl;
            im.setPC(im.getPC() + 1); // move to next instruction
        }

    return 0;
}