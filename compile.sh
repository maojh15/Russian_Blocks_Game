#!/bin/bash
INCLUDES="-I/home/maojh/thirdPartCLib/include"
LIBS="-lSDL2 -lSDL2_image -lSDL2_ttf"
g++ -std=c++14 ${INCLUDES} -c main.cpp -o main.o ${LIBS}
g++ -std=c++14 ${INCLUDES} -c RussianBlockGame.cpp -o RussianBlockGame.o ${LIBS}
g++ main.o RussianBlockGame.o -o main ${LIBS}


