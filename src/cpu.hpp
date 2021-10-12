#pragma once
#include "structs.hpp"
#include "enums.hpp"
#include "ansiColors.hpp"

#include <stdexcept>
#include <iostream>

class CPU {
public:
    Program currentProgram;
    InterpreterOptions options;
    std::vector<LL> memory; // 8mb should be just enough

    LL pc;
    bool running = true;
    bool incrementPc = true;

    CPU(const InterpreterOptions& opts, const Program& program) {
        pc = 0;
        currentProgram = program;
        options = opts;
        memory.resize(1048576);
    }

    void Begin() {
        while (running) {
            executeNextInstruction();
            if (incrementPc) {
                pc++;
            }
            
            if (pc == currentProgram.instructions.size()) {
                running = false;
                
                break;
            }
        }
    }
    void DumpMemoryAddress(LL mem) {
        std::cout << "memory[" << mem << "]: " << memory[mem] << std::endl;
    }

private:

    LL* processArgLLType(Instruction* instr) {
        switch (instr->argType) {
            case ArgumentType::CONSTANT: {
                // e.g. load =2
                return &instr->argi;
            }
            case ArgumentType::CONSTANTMEMORYREF: {
                // e.g. load 2
                return &memory[instr->argi];
            }
            case ArgumentType::CONSTANTPOINTER: {
                // e.g. load ^2
                return &memory[memory[instr->argi]];
            }
        }
    }

    LL* processArgLLTypeXConst(Instruction* instr) {
        switch (instr->argType) {
            case ArgumentType::CONSTANT: {
                // e.g. load =2
                std::cout << "raminterpreter: " << RED << "error" << RESET << ": cannot store to a constant, expected MEMORYREF or POINTER" << std::endl;
                throw std::runtime_error("bad instruction argument, got CONSTANT, expected MEMORYREF or POINTER");
            }
            case ArgumentType::CONSTANTMEMORYREF: {
                // e.g. load 2
                return &instr->argi;
            }
            case ArgumentType::CONSTANTPOINTER: {
                // e.g. load ^2
                return &memory[instr->argi];
            }
        }
    }

    void executeNextInstruction() {
        incrementPc = true;
        Instruction* instr = &currentProgram.instructions[pc];
        switch (instr->type) {
            case CPUInstructionType::LOAD: {
                LL* memloc = processArgLLType(instr);
                memory[0] = *memloc;
                break;
            }
            case CPUInstructionType::STORE: {
                LL* memloc = processArgLLTypeXConst(instr);
                memory[*memloc] = memory[0];
                break;
            }
            case CPUInstructionType::ADD: {
                LL* memloc = processArgLLType(instr);
                memory[0] += *memloc;
                break;
            }
            case CPUInstructionType::SUB: {
                LL* memloc = processArgLLType(instr);
                memory[0] -= *memloc;
                break;
            }
            case CPUInstructionType::MULT: {
                LL* memloc = processArgLLType(instr);
                memory[0] *= *memloc;
                break;
            }
            case CPUInstructionType::DIV: {
                LL* memloc = processArgLLType(instr);
                memory[0] /= *memloc;
                break;
            }
            case CPUInstructionType::READ: {
                LL i;
                std::cin >> i;
                LL* memloc = processArgLLTypeXConst(instr);
                memory[*memloc] = i;
                break;
            }
            case CPUInstructionType::WRITE: {
                LL* memloc = processArgLLType(instr);
                std::cout << *memloc << std::endl;
                break;
            }
            case CPUInstructionType::JUMP: {
                LL* jumploc = &currentProgram.labelReferences.find(instr->args)->second;
                pc = *jumploc;
                incrementPc = false;
                break;
            }
            case CPUInstructionType::JGTZ: {
                if (memory[0] > 0) {
                    LL* jumploc = &currentProgram.labelReferences.find(instr->args)->second;
                    pc = *jumploc;
                    incrementPc = false;
                }
                break;
            }
            case CPUInstructionType::JZERO: {
                if (memory[0] == 0) {
                    LL* jumploc = &currentProgram.labelReferences.find(instr->args)->second;
                    pc = *jumploc;
                    incrementPc = false;
                }
                break;
            }
            case CPUInstructionType::HALT: {
                running = false;
                incrementPc = false;
                break;
            }
        }
    }
};