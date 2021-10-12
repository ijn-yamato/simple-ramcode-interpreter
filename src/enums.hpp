#pragma once

enum class CPUInstructionType {
    LOAD = 0,
    STORE = 1,
    ADD = 2,
    SUB = 3,
    MULT = 4,
    DIV = 5,
    READ = 6,
    WRITE = 7,
    JUMP = 8,
    JGTZ = 9,
    JZERO = 10,
    HALT = 11
};

enum class ArgumentType {
    LABELREF = 0,
    CONSTANT = 1,
    CONSTANTMEMORYREF = 2,
    CONSTANTPOINTER = 3
};