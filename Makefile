OBJS = main.cpp RussianBlockGame.cpp
HEADS = RussianBlockGame.h RussianBlockGameUI.h
OBJ_NAME = play
CC = g++
COMPILER_FLAGS = -std=c++14 -O3
INCLUDES = -I./include
LIBARARIES = -L./libs
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -llua

${OBJ_NAME} : ${OBJS} ${HEADS}
	${CC} ${COMPILER_FLAGS} ${INCLUDES} ${OBJS} -o ${OBJ_NAME} ${LIBARARIES} ${LIBS}
