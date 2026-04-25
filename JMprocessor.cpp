#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <array>
#include <iomanip>
#include <bitset>
#include <algorithm>
#include <stdexcept>
#include <cctype>
#include <cstdint>

using namespace std;

namespace processor {

    // ─── Opcode Enum ──────────────────────────────────────────────────────────
    enum class Opcode {
        ADD, ADDI, SUB, MUL, AND, OR, SLL, SRL, LW, SW, BEQ, J, NOP, INVALID
    };

    static string opcodeToString(Opcode op) {
        switch (op) {
            case Opcode::ADD:  return "ADD";
            case Opcode::ADDI: return "ADDI";
            case Opcode::SUB:  return "SUB";
            case Opcode::MUL:  return "MUL";
            case Opcode::AND:  return "AND";
            case Opcode::OR:   return "OR";
            case Opcode::SLL:  return "SLL";
            case Opcode::SRL:  return "SRL";
            case Opcode::LW:   return "LW";
            case Opcode::SW:   return "SW";
            case Opcode::BEQ:  return "BEQ";
            case Opcode::J:    return "J";
            case Opcode::NOP:  return "NOP";
            default:           return "INVALID";
        }
    }

    // ─── Decoded Instruction ─────────────────────────────────────────────────
    struct Instruction {
        Opcode opcode = Opcode::NOP;
        string raw;
        string label;
        int rs = 0, rt = 0, rd = 0;
        int imm = 0, shamt = 0, target = -1;
        bool valid = false;
    };

    // ─── Control Signals ─────────────────────────────────────────────────────
    struct ControlSignals {
        int RegDst   = 0;
        int ALUSrc   = 0;
        int MemtoReg = 0;
        int RegWrite = 0;
        int MemRead  = 0;
        int MemWrite = 0;
        int Branch   = 0;
        int Jump     = 0;
        string ALUOp = "ADD";
    };

    // ─── Pipeline State Registers ─────────────────────────────────────────────
    struct IF_ID {
        bool valid = false;
        int pc = 0;
        Instruction instr;
    };

    struct ID_EX {
        bool valid = false;
        int pc = 0;
        Instruction instr;
        ControlSignals control;
        int readData1 = 0, readData2 = 0;
        int signExtImm = 0;
        int rs = 0, rt = 0, rd = 0, shamt = 0;
    };

    struct EX_MEM {
        bool valid = false;
        int pc = 0;
        Instruction instr;
        ControlSignals control;
        int aluResult = 0, writeData = 0, writeReg = 0;
        bool zero = false, branchTaken = false, jumpTaken = false;
        int branchTarget = 0, jumpTarget = 0;
    };

    struct MEM_WB {
        bool valid = false;
        int pc = 0;
        Instruction instr;
        ControlSignals control;
        int readData = 0, aluResult = 0, writeReg = 0;
    };

    // ─── Helpers ─────────────────────────────────────────────────────────────
    static string trim(const string& s) {
        size_t a = s.find_first_not_of(" \t\r\n"); //length of whitespace at start of string
        if (a == string::npos) return ""; //change s to be substring starting at first non-whitespace character
        return s.substr(a, s.find_last_not_of(" \t\r\n") - a + 1); //change s to be substring starting at first non-whitespace character and ending at last non-whitespace character
    }

    static string toUpper(string s) {
        transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return toupper(c); });
        return s;
    }

    static vector<string> splitCSV(const string& line) {
        vector<string> parts;
        string token;
        istringstream ss(line);

        while (getline(ss, token, ',')) {
        size_t pos = token.find(';');

        if (pos != string::npos) {
            token = token.substr(0, pos);
            parts.push_back(trim(token));
            break; // stop parsing after comment marker
        }

        parts.push_back(trim(token));
    }

        return parts;
    }

    static int parseReg(string r) {
        r = trim(r);
        if (!r.empty() && r[0] == '$') r = r.substr(1);
        if (!r.empty() && (r[0]=='R'||r[0]=='r')) r = r.substr(1);
        int v = stoi(r);
        if (v < 0 || v > 31) throw out_of_range("Register out of range: " + r);
        return v;
    }

    static Opcode parseOpcode(const string& op) {
        string u = toUpper(trim(op));
        if (u=="ADD")  return Opcode::ADD;
        if (u=="ADDI") return Opcode::ADDI;
        if (u=="SUB")  return Opcode::SUB;
        if (u=="MUL")  return Opcode::MUL;
        if (u=="AND")  return Opcode::AND;
        if (u=="OR")   return Opcode::OR;
        if (u=="SLL")  return Opcode::SLL;
        if (u=="SRL")  return Opcode::SRL;
        if (u=="LW")   return Opcode::LW;
        if (u=="SW")   return Opcode::SW;
        if (u=="BEQ")  return Opcode::BEQ;
        if (u=="J")    return Opcode::J;
        if (u=="NOP")  return Opcode::NOP;
        return Opcode::INVALID;
    }

    // ─── Register File ────────────────────────────────────────────────────────
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

    // ─── ALU ─────────────────────────────────────────────────────────────────
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

    // ─── Data Memory ─────────────────────────────────────────────────────────
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

    // ─── Control Unit ────────────────────────────────────────────────────────
    class Control_Unit {
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
    };

    // ─── Instruction Memory ──────────────────────────────────────────────────
    class Instruction_Memory {
    private:
        int pc = 0;
        int SIZE;
        vector<Instruction> instruction_set;
        unordered_map<string,int> label_map;

        Instruction parseInstruction(const string& line) {
            Instruction inst;
            inst.raw = line;
            auto parts = splitCSV(line);
            
            if (parts.empty()) return inst;
            inst.opcode = parseOpcode(parts[0]);
            if (inst.opcode == Opcode::INVALID)
                throw runtime_error("Unknown opcode: " + parts[0]);

            switch (inst.opcode) { // for stoi calls, removed trim since splitCSV already trims the parts
                case Opcode::ADD: case Opcode::SUB: case Opcode::MUL:
                case Opcode::AND: case Opcode::OR:
                    if (parts.size()!=4) throw runtime_error("Bad format: " + line);
                    inst.rd=parseReg(parts[1]); inst.rs=parseReg(parts[2]); inst.rt=parseReg(parts[3]); break;
                case Opcode::ADDI:
                    if (parts.size()!=4) throw runtime_error("Bad format: " + line);
                    inst.rt=parseReg(parts[1]); inst.rs=parseReg(parts[2]); inst.imm=stoi(parts[3]); break;
                case Opcode::SLL: case Opcode::SRL:
                    if (parts.size()!=4) throw runtime_error("Bad format: " + line);
                    inst.rd=parseReg(parts[1]); inst.rt=parseReg(parts[2]); inst.shamt=stoi(parts[3]); break;
                case Opcode::LW: case Opcode::SW:
                    if (parts.size()!=4) throw runtime_error("Bad format: " + line);
                    inst.rt=parseReg(parts[1]); inst.imm=stoi(parts[2]); inst.rs=parseReg(parts[3]); break; 
                case Opcode::BEQ:
                    if (parts.size()!=4) throw runtime_error("Bad format: " + line);
                    inst.rs=parseReg(parts[1]); inst.rt=parseReg(parts[2]); inst.label=trim(parts[3]); break;
                case Opcode::J:
                    if (parts.size()!=2) throw runtime_error("Bad format: " + line);
                    inst.label=trim(parts[1]); break;
                case Opcode::NOP: break;
                default: break;
            }
            inst.valid = true;
            return inst;
        }

    public:
        explicit Instruction_Memory(int size=256) : SIZE(size) { instruction_set.reserve(size); }

        int getPC() const { return pc; }
        void setPC(int newPC) { if (newPC<0||newPC>(int)instruction_set.size()) throw out_of_range("PC OOB"); pc=newPC; }
        int size()  const { return (int)instruction_set.size(); }
        Instruction getInstructionAt(int i) const { if(i<0||i>=(int)instruction_set.size()) return {}; return instruction_set[i]; }
        string getInstruction() const { if(pc<0||pc>=(int)instruction_set.size()) return ""; return instruction_set[pc].raw; }

        void loadInstructions(ifstream& infile) {
            vector<string> lines;
            string line;
            int addr = 0;
            while (getline(infile, line)) {
                size_t cp = line.find('#');
                if (cp != string::npos) line = line.substr(0, cp);
                line = trim(line);
                if (line.empty()) continue;
                size_t col = line.find(':');
                if (col != string::npos) {
                    label_map[trim(line.substr(0,col))] = addr;
                    line = trim(line.substr(col+1));
                    if (line.empty()) continue;
                }
                lines.push_back(line);
                if (++addr > SIZE) throw out_of_range("Instruction memory overflow");
            }
            for (auto& l : lines) instruction_set.push_back(parseInstruction(l));
            for (auto& inst : instruction_set)
                if ((inst.opcode==Opcode::BEQ||inst.opcode==Opcode::J) && !inst.label.empty()) {
                    if (!label_map.count(inst.label)) throw runtime_error("Undefined label: "+inst.label);
                    inst.target = label_map[inst.label];
                }
        }

        uint32_t encodeInstruction(const Instruction& inst) const {
            switch (inst.opcode) {
                case Opcode::ADD:  return (0u<<26)|(inst.rs<<21)|(inst.rt<<16)|(inst.rd<<11)|32u;
                case Opcode::SUB:  return (0u<<26)|(inst.rs<<21)|(inst.rt<<16)|(inst.rd<<11)|34u;
                case Opcode::AND:  return (0u<<26)|(inst.rs<<21)|(inst.rt<<16)|(inst.rd<<11)|36u;
                case Opcode::OR:   return (0u<<26)|(inst.rs<<21)|(inst.rt<<16)|(inst.rd<<11)|37u;
                case Opcode::SLL:  return (0u<<26)|(inst.rt<<16)|(inst.rd<<11)|((inst.shamt&0x1F)<<6)|0u;
                case Opcode::SRL:  return (0u<<26)|(inst.rt<<16)|(inst.rd<<11)|((inst.shamt&0x1F)<<6)|2u;
                case Opcode::MUL:  return (28u<<26)|(inst.rs<<21)|(inst.rt<<16)|(inst.rd<<11)|2u;
                case Opcode::ADDI: return (8u<<26)|(inst.rs<<21)|(inst.rt<<16)|(inst.imm&0xFFFF);
                case Opcode::LW:   return (35u<<26)|(inst.rs<<21)|(inst.rt<<16)|(inst.imm&0xFFFF);
                case Opcode::SW:   return (43u<<26)|(inst.rs<<21)|(inst.rt<<16)|(inst.imm&0xFFFF);
                case Opcode::BEQ:  return (4u<<26)|(inst.rs<<21)|(inst.rt<<16)|((inst.target-1)&0xFFFF);
                case Opcode::J:    return (2u<<26)|(inst.target&0x03FFFFFF);
                case Opcode::NOP:  return 0u;
                default:           return 0u;
            }
        }

        void printBinaryListing() const {
            cout << "╔══════════════════ Binary Listing ═══════════════════╗\n";
            cout << "║  PC   Binary Encoding                   Instruction  ║\n";
            cout << "╠════════════════════════════════════════════════════════╣\n";
            for (int i=0;i<(int)instruction_set.size();++i) {
                uint32_t enc = encodeInstruction(instruction_set[i]);
                cout << "║ [" << setw(2) << i << "]  " << bitset<32>(enc)
                     << "  " << left << setw(24) << instruction_set[i].raw << right << " ║\n";
            }
            cout << "╚══════════════════════════════════════════════════════╝\n\n";
        }
    };

    // ─── Pipeline Simulator ──────────────────────────────────────────────────
    class PipelineSimulator {
    private:
        Instruction_Memory& im;
        Register_File rf;
        ALU alu;
        Data_Memory dm;
        Control_Unit cu;

        IF_ID  if_id,  if_id_next;
        ID_EX  id_ex,  id_ex_next;
        EX_MEM ex_mem, ex_mem_next;
        MEM_WB mem_wb, mem_wb_next;

        bool debugMode    = false;
        bool redirectPC   = false;
        int  redirectedPC = 0;

        bool pipelineEmpty() const {
            return !if_id.valid && !id_ex.valid && !ex_mem.valid && !mem_wb.valid;
        }

        // Helper to format control signals for debug printing
        static string ctrlStr(const ControlSignals& c) {
            ostringstream os;
            os << "RegDst="   << c.RegDst
               << " ALUSrc="  << c.ALUSrc
               << " MemtoReg="<< c.MemtoReg
               << " RegWrite="<< c.RegWrite
               << " MemRead=" << c.MemRead
               << " MemWrite="<< c.MemWrite
               << " Branch="  << c.Branch
               << " Jump="    << c.Jump
               << " ALUOp="   << c.ALUOp;
            return os.str();
        }

        // Helper to format instruction for debug printing
        static string instrStr(const Instruction& inst, bool v) {
            if (!v) return "[ empty ]";
            return inst.raw;
        }

        // ── WB Stage ──────────────────────────────────────────────────────────
        void stageWB() {
            if (!mem_wb.valid) return; //make sure that state register has had a chance to update with correct data before we read it
            //check mem_wb control signals to determine if we need to write back to register file and if what we write back is coming from memory or ALU result
            if (mem_wb.control.RegWrite) {
                int val = mem_wb.control.MemtoReg ? mem_wb.readData : mem_wb.aluResult;
                rf.write(mem_wb.writeReg, val);
            }
        }

        // ── MEM Stage ─────────────────────────────────────────────────────────
        void stageMEM() {
            mem_wb_next = {};
            if (!ex_mem.valid) return; //make sure that state register has had a chance to update with correct data before we read it
            mem_wb_next.valid     = true;
            //propagate state register data to next stage
            mem_wb_next.pc        = ex_mem.pc;
            mem_wb_next.instr     = ex_mem.instr;
            mem_wb_next.control   = ex_mem.control;
            mem_wb_next.aluResult = ex_mem.aluResult;
            mem_wb_next.writeReg  = ex_mem.writeReg;

            //perform memory operations if called for by control signals
            if (ex_mem.control.MemRead)  mem_wb_next.readData = dm.loadWord(ex_mem.aluResult);
            if (ex_mem.control.MemWrite) dm.storeWord(ex_mem.aluResult, ex_mem.writeData);

            //handle branch/jump redirection - if branch is taken or jump is taken, we need to redirect the PC and flush the instructions in IF and ID stages
            if (ex_mem.branchTaken) { redirectPC=true; redirectedPC=ex_mem.branchTarget; if_id_next={}; id_ex_next={}; ex_mem_next={}; }
            if (ex_mem.jumpTaken)   { redirectPC=true; redirectedPC=ex_mem.jumpTarget;   if_id_next={}; id_ex_next={}; ex_mem_next={}; }
        }

        // ── EX Stage ──────────────────────────────────────────────────────────
        void stageEX() {
            ex_mem_next = {};
            if (!id_ex.valid) return; //make sure that state register has had a chance to update with correct data before we read it
            ex_mem_next.valid     = true;
            //propagate state register data to next stage
            ex_mem_next.pc        = id_ex.pc;
            ex_mem_next.instr     = id_ex.instr;
            ex_mem_next.control   = id_ex.control;
            ex_mem_next.writeData = id_ex.readData2;
            ex_mem_next.writeReg  = id_ex.control.RegDst ? id_ex.rd : id_ex.rt;

            // perform ALU operation using control signals to determine ALU inputs
            int opB = id_ex.control.ALUSrc ? id_ex.signExtImm : id_ex.readData2;
            ex_mem_next.aluResult   = alu.execute(id_ex.control.ALUOp, id_ex.readData1, opB, id_ex.shamt);
            ex_mem_next.zero        = (ex_mem_next.aluResult == 0);
            ex_mem_next.branchTarget = id_ex.instr.target;
            ex_mem_next.jumpTarget   = id_ex.instr.target;
            ex_mem_next.branchTaken  = id_ex.control.Branch && ex_mem_next.zero;
            ex_mem_next.jumpTaken    = id_ex.control.Jump;
        }

        // ── ID Stage ──────────────────────────────────────────────────────────
        void stageID() {
            id_ex_next = {};
            if (!if_id.valid) return; //make sure that state register has had a chance to update with correct data before we read it
            id_ex_next.valid       = true;
            //propagate state register data to next stage
            id_ex_next.pc          = if_id.pc;
            id_ex_next.instr       = if_id.instr;
            id_ex_next.rs          = if_id.instr.rs;
            id_ex_next.rt          = if_id.instr.rt;
            id_ex_next.rd          = if_id.instr.rd;
            id_ex_next.shamt       = if_id.instr.shamt;
            id_ex_next.signExtImm  = if_id.instr.imm;
            //read registers and generate control signals
            id_ex_next.readData1   = rf.read(if_id.instr.rs);
            id_ex_next.readData2   = rf.read(if_id.instr.rt);
            id_ex_next.control     = cu.decode(if_id.instr.opcode);
        }

        // ── IF Stage ──────────────────────────────────────────────────────────
        void stageIF() {
            if_id_next = {};
            if (redirectPC) { im.setPC(redirectedPC); redirectPC=false; } //take branch/jump if called for by MEM stage
            //fetch next instruction if PC is valid and increment PC for next cycle.
            if (im.getPC() < im.size()) {
                if_id_next.valid = true;
                if_id_next.pc   = im.getPC();
                if_id_next.instr= im.getInstructionAt(im.getPC());
                im.setPC(im.getPC()+1);
            }
        }

        void commit() { if_id=if_id_next; id_ex=id_ex_next; ex_mem=ex_mem_next; mem_wb=mem_wb_next; }

        void printDebugState(int cycle) const {
            cout << "\n╔══════════════════════ Cycle " << setw(3) << cycle << " ══════════════════════╗\n";

            // Pipeline state registers
            cout << "║  Pipeline State Registers                              ║\n";
            cout << "╠════════════════════════════════════════════════════════╣\n";
            cout << "║  IF/ID  ▶ " << left << setw(44) << instrStr(if_id_next.instr, if_id_next.valid) << right << " ║\n";
            cout << "║  ID/EX  ▶ " << left << setw(44) << instrStr(id_ex_next.instr, id_ex_next.valid) << right << " ║\n";
            cout << "║  EX/MEM ▶ " << left << setw(44) << instrStr(ex_mem_next.instr, ex_mem_next.valid) << right << " ║\n";
            cout << "║  MEM/WB ▶ " << left << setw(44) << instrStr(mem_wb_next.instr, mem_wb_next.valid) << right << " ║\n";

            // ID/EX detailed values
            if (id_ex_next.valid) {
                cout << "╠════════════════════════════════════════════════════════╣\n";
                cout << "║  ID/EX Data:                                           ║\n";
                cout << "║    ReadData1 = " << setw(10) << id_ex_next.readData1
                     << "   ReadData2 = " << setw(10) << id_ex_next.readData2 << "         ║\n";
                cout << "║    SignExtImm= " << setw(10) << id_ex_next.signExtImm
                     << "   Shamt    = " << setw(10) << id_ex_next.shamt << "         ║\n";
                cout << "║    RS=" << setw(2) << id_ex_next.rs
                     << "  RT=" << setw(2) << id_ex_next.rt
                     << "  RD=" << setw(2) << id_ex_next.rd << "                                ║\n";
            }

            // EX/MEM detailed values
            if (ex_mem_next.valid) {
                cout << "╠════════════════════════════════════════════════════════╣\n";
                cout << "║  EX/MEM Data:                                          ║\n";
                cout << "║    ALUResult= " << setw(10) << ex_mem_next.aluResult
                     << "   WriteReg = " << setw(10) << ex_mem_next.writeReg << "         ║\n";
                cout << "║    Zero=" << ex_mem_next.zero
                     << "  BranchTaken=" << ex_mem_next.branchTaken
                     << "  JumpTaken=" << ex_mem_next.jumpTaken << "            ║\n";
            }

            // MEM/WB detailed values
            if (mem_wb_next.valid) {
                cout << "╠════════════════════════════════════════════════════════╣\n";
                cout << "║  MEM/WB Data:                                          ║\n";
                cout << "║    ALUResult= " << setw(10) << mem_wb_next.aluResult
                     << "   ReadData = " << setw(10) << mem_wb_next.readData << "         ║\n";
                cout << "║    WriteReg = " << setw(10) << mem_wb_next.writeReg << "                          ║\n";
            }

            // Control signals (from ID/EX — where control unit outputs are generated)
            if (id_ex_next.valid) {
                cout << "╠════════════════════════════════════════════════════════╣\n";
                cout << "║  Control Signals (ID/EX):                              ║\n";
                cout << "║    " << left << setw(51) << ctrlStr(id_ex_next.control) << right << " ║\n";
            }

            cout << "╠════════════════════════════════════════════════════════╣\n";
            rf.print();
            cout << "╚══════════════════════════════════════════════════════╝\n";
        }

    public:
        PipelineSimulator(Instruction_Memory& memory, bool debug=false)
            : im(memory), debugMode(debug) {}

        Register_File& registers() { return rf; }
        Data_Memory&   dataMemory() { return dm; }

        void run() {
            int cycle = 1;
            while (im.getPC() < im.size() || !pipelineEmpty()) {
                //backward order to avoid overwriting state before it's used in the same cycle
                stageWB();
                stageMEM();
                stageEX();
                stageID();
                stageIF();
                if (debugMode) printDebugState(cycle);
                commit();
                cycle++;
            }
        }
    };

} // namespace processor
