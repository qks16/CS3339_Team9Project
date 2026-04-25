#include <iostream>
#include <string>
#include <fstream>
#include "newprocessor.cpp"

using namespace std;
using namespace processor;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: ./im_test <instruction_memory_size> <assembly_file>\n";
        return 1;
    }

    int instruction_memory_size = atoi(argv[1]);
    Instruction_Memory im(instruction_memory_size);
    ifstream infile(argv[2]);

    if (!infile.is_open()) {
        cerr << "Could not open file.\n";
        return 1;
    }

    im.loadInstructions(infile);
    im.printBinaryListing();

    while (im.getPC() < im.size()) {
        cout << "Executing instruction at PC: " << im.getPC() << " -> " << im.getInstruction() << endl;
        im.setPC(im.getPC() + 1);
    }

    return 0;
}
