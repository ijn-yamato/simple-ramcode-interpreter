#include <iostream>
#include <fstream>
#include <regex>
#include <string>   // C++ header
#include <string.h> // C header
#include "ansiColors.hpp"
#include "enums.hpp"
#include "structs.hpp"
#include "cpu.hpp"

std::vector<std::string> stringSeparate(std::string str, std::string token){
    std::vector<std::string>result;
    while(str.size()){
        int index = str.find(token);
        if(index!=std::string::npos){
            result.push_back(str.substr(0,index));
            str = str.substr(index+token.size());
            if(str.size()==0)result.push_back(str);
        }else{
            result.push_back(str);
            str = "";
        }
    }
    return result;
}


std::string removeDuplicateSpaces(const std::string& string) {
    std::regex spaceRg(R"(\s+)");
    return std::regex_replace(string, spaceRg, " ");
}

std::string removeFirstSpace(const std::string& string) {
    auto pos = string.find_first_not_of(' ');
    return string.substr(pos != std::string::npos ? pos : 0);
}

int main(int args, char *argv[]) {
    InterpreterOptions options{};
    std::string ramcodeSource = "";

    for (int i = 1; i < args; i++) {
        const char *option = argv[i];
        if (strcmp(option, "--o") == 0) {
            if ((i+1) >= args) {
                std::cout << "raminterpreter: " << RED << "error" << RESET << ": invalid output file: argument index out of bounds" << std::endl;
                return 1;
            }
            i++;
            const char* file = argv[i];
            options.outputToFile = true;
            options.outputFile = std::string(file);
        }

        std::string optionCpp = std::string(option);
        if (optionCpp.rfind("--", 0) != 0) {
            if (optionCpp.empty()) {
                std::cout << "raminterpreter: " << RED << "error" << RESET << ": more than one ramcode source provided" << std::endl;
                return 1;
            }
            ramcodeSource = optionCpp;
        }
    }

    Program ldProg{};

    std::ifstream progFile(ramcodeSource, std::ios::ate);

    if (!progFile.is_open()) {
        std::cout << "raminterpreter: " << RED << "error" << RESET << ": failed to open ramcode file" << std::endl;
        return 1;
    }

    size_t filesize = progFile.tellg();
    std::vector<char> buffer(filesize);

    progFile.seekg(0);
    if (buffer.size() == filesize && &buffer != nullptr) { // CWE-120 check
        progFile.read(buffer.data(), filesize);
    } else {
        std::cout << "raminterpreter: " << RED << "error" << RESET << ": failed to create program buffer" << std::endl;
        return 1;
    }
    progFile.close();

    // parse stuff

    std::string stringifiedProg = std::string(buffer.data());

    std::vector<std::string> instructions = stringSeparate(stringifiedProg, "\n");

    LL counter = 0;

    for (std::string instruction : instructions) {
        std::string trunc = removeDuplicateSpaces(instruction);
        std::string ncomment = stringSeparate(trunc, "#")[0];
        std::vector<std::string> label = stringSeparate(ncomment, ":");
        Instruction instr;
        std::string nspaceInstr = removeFirstSpace(label[label.size()-1]);
        std::vector<std::string> instructionData = stringSeparate(nspaceInstr, " ");
        instr.argType = ArgumentType::CONSTANT;
        
        if (instructionData[0] == "load") {
            instr.type = CPUInstructionType::LOAD;
            instr.argType = ArgumentType::CONSTANT;
        } else if (instructionData[0] == "store") {
            instr.type = CPUInstructionType::STORE;
            instr.argType = ArgumentType::CONSTANT;
        } else if (instructionData[0] == "add") {
            instr.type = CPUInstructionType::ADD;
            instr.argType = ArgumentType::CONSTANT;
        } else if (instructionData[0] == "sub") {
            instr.type = CPUInstructionType::SUB;
            instr.argType = ArgumentType::CONSTANT;
        } else if (instructionData[0] == "mult") {
            instr.type = CPUInstructionType::MULT;
            instr.argType = ArgumentType::CONSTANT;
        } else if (instructionData[0] == "div") {
            instr.type = CPUInstructionType::DIV;
            instr.argType = ArgumentType::CONSTANT;
        } else if (instructionData[0] == "read") {
            instr.type = CPUInstructionType::READ;
            instr.argType = ArgumentType::CONSTANT;
        } else if (instructionData[0] == "write") {
            instr.type = CPUInstructionType::WRITE;
            instr.argType = ArgumentType::CONSTANT;
        } else if (instructionData[0] == "jump") {
            instr.type = CPUInstructionType::JUMP;
            instr.argType = ArgumentType::LABELREF;
        } else if (instructionData[0] == "jgtz") {
            instr.type = CPUInstructionType::JGTZ;
            instr.argType = ArgumentType::LABELREF;
        } else if (instructionData[0] == "jzero") {
            instr.type = CPUInstructionType::JZERO;
            instr.argType = ArgumentType::LABELREF;
        } else if (instructionData[0] == "halt") {
            instr.type = CPUInstructionType::HALT;
            instr.argType = ArgumentType::LABELREF;
        } 
        if (instr.argType == ArgumentType::CONSTANT) {
            LL ai;
            if (instructionData[1].rfind("^", 0) == 0) {
                instr.argType = ArgumentType::CONSTANTPOINTER;
                ai = strtoll(instructionData[1].substr(1).c_str(), NULL, 10);
            } else if (instructionData[1].rfind("=", 0) == 0) {
                instr.argType = ArgumentType::CONSTANT;
                ai = strtoll(instructionData[1].substr(1).c_str(), NULL, 10);
            } else {
                instr.argType = ArgumentType::CONSTANTMEMORYREF;
                ai = strtoll(instructionData[1].c_str(), NULL, 10);
            }
            instr.argi = ai;
        } else {
            instr.args = instructionData[1];
        }
        //std::cout << instr.argi << std::endl;
        //std::cout << instr.type << std::endl;
        //std::cout << instr.argType << std::endl;
        
        ldProg.instructions.push_back(instr);
        if (label.size() > 1) {
            ldProg.labelReferences.insert(std::make_pair(label[0], counter));
        }
        counter++;
    }



    CPU xCpu = CPU(options, ldProg);

    xCpu.Begin();

    return 0;
}