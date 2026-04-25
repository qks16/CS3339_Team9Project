#include <iostream>
#include <fstream>
#include <string>
#include "JMprocessor.cpp" //changed to link different file MIGHT BE WRONG

using namespace std;
using namespace processor;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: ./mips_sim <instruction_memory_size> <assembly_file> [--debug]\n";
        cerr << "Example: ./mips_sim 256 program.asm\n";
        cerr << "Example: ./mips_sim 256 program.asm --debug\n";
        return 1;
    }

    try {
        int instruction_memory_size = atoi(argv[1]);
        ifstream infile(argv[2]);
        if (!infile.is_open()) {
            cerr << "Error: Could not open input file: " << argv[2] << '\n';
            return 1;
        }

        bool debug = (argc >= 4 && string(argv[3]) == "--debug");

        Instruction_Memory im(instruction_memory_size);
        im.loadInstructions(infile);
        infile.close();

        im.printBinaryListing();

        PipelineSimulator sim(im, debug);
        sim.run();

        cout << "\n================================================\n";
        cout << "Final Machine State After Program Execution\n";
        cout << "================================================\n";
        sim.registers().print();
        sim.dataMemory().print();
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
