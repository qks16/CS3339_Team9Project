# CS3339 — MIPS Pipeline Simulator
**CS3339 Computer Architecture · Spring 2026 · Team 9**

A 5-stage pipelined MIPS processor simulator built in C++. Simulates the IF → ID → EX → MEM → WB pipeline and outputs the final register file and memory state. Verified on Windows 11 using MINGW64 (GCC 15.2.0).

---

## Build

```bash
g++ -std=c++17 -c newprocessor.cpp
g++ -std=c++17 -Wall newsource.cpp newprocessor.o -o mips_sim.exe
```

---

## Run

**Normal mode:**
```bash
./mips_sim.exe 256 assemblydemo2
```

**Debug mode** (shows pipeline registers and control signals each cycle):
```bash
./mips_sim.exe 256 assemblydemo2 --debug
```

---

## Supported Instructions
`ADD` `ADDI` `SUB` `MUL` `AND` `OR` `SLL` `SRL` `LW` `SW` `BEQ` `J` `NOP`

---

## Assembly Format
- Comma-separated fields, one instruction per line
- Labels: `done: NOP`
- Insert **3 NOPs** between dependent instructions (no forwarding)

---

## Project Structure

```text
CS3339_Team9Project/
├── newsource.cpp       — main entry point, parses args and runs simulator
├── newprocessor.cpp    — PipelineSimulator, ALU, Register_File,
│                          Instruction_Memory, Data_Memory, Control_Unit
├── newIM_tester.cpp    — standalone instruction memory tester
├── Makefile            — build rules for mips_sim.exe and tester
└── assemblydemo2       — demo program using all 13 instructions
