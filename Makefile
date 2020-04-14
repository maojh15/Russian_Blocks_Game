OBJS = main.cpp RussianBlockGame.cpp
HEADS = RussianBlockGame.h RussianBlockGameUI.h
OBJ_NAME = play
CC = g++
COMPILER_FLAGS = -std=c++14 -g
INCLUDES = -I/home/maojh/thirdPartCLib/include
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -llua

${OBJ_NAME} : ${OBJS} ${HEADS}
	${CC} ${COMPILER_FLAGS} ${INCLUDES} ${OBJS} -o ${OBJ_NAME} ${LIBS}
