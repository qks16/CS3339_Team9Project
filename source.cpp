#include <iostream>
#include <string>
#include <fstream>

#include "processor.cpp"

using namespace std;

int main(int argc, char* argv[]) {

    //create object for each component of the processor,

    //create instruction memory object with size specified by command line argument
    int instruction_memory_size = atoi(argv[1]);
    processor::Instruction_Memory im(instruction_memory_size); // create an instruction memory with size specified by command line argument
    
    ifstream infile(argv[2]);

    //load instructions from file into instruction memory
    im.loadInstructions(infile);

    im.setPC(0); // reset PC to 0


    //execute instructions in instruction memory until we reach the end of the program (when PC reaches the size of instruction memory)
    while (im.getPC() < instruction_memory_size) {
        cout << "Executing instruction at PC: " << im.getPC() << " - " << im.getInstruction() << endl;

        //find instruction components by parsing the instruction string using comma as a delimiter based on OPCODE

        //impliment register file and pass in registers specified in the instruction string and control signals from the control unit
    }

    return 0;
}