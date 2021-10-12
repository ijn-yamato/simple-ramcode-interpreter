#pragma once
#include <string>
#include <vector>
#include <map>

#define LL long long

struct InterpreterOptions {
    bool dumpMemoryAddresses = false;
    std::string outputFile;
    bool outputToFile = false;
};

struct Instruction {
    std::string label;
    CPUInstructionType type;
    ArgumentType argType;
    std::string args;
    LL argi;
};

struct Program {
    std::vector<Instruction> instructions;
    std::map<std::string, LL> labelReferences;
};