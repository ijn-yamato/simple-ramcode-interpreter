CC = g++
CFLAGS = -std=c++17 -ggdb
buildDst = ./raminterpreter

BuildRCInt:
	${CC} ${CFLAGS} ./src/ramcodeinterpreter.cpp -o ${buildDst}