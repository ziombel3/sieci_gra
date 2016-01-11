LIBS=-lGL -lglut -lGLEW
INCPATH  	= -I../include -I/usr/local/include -I/usr/X11R6/include -I/usr/include -I/usr/include/freetype2 
ELSE= -fno-strict-aliasing  -L/usr/X11R6/lib  -lGL -lGLU -lX11 -lXext -lXmu -lXrandr -lpthread -lm -lGLEW -lfreetype  -L/usr/local/lib -llua -lm -ldl -L/usr/lib/nvidia-340  `pkg-config --cflags assimp` -g  -MMD -MP
CC=g++

SOURCES=main_file.cpp tga.cpp shaderprogram.cpp OBJ.cpp player.cpp mouse.cpp common/shader_utils.cpp text.cpp
HEADERS=tga.h cube.h teapot.h shaderprogram.h OBJ.h player.h mouse.h common/shader_utils.cpp text.h
OBJECTS=$(SOURCES:.cpp=.o)

all: main_file

main_file: $(OBJECTS) 
	$(CC) -o ./run $(OBJECTS) $(LIBS) $(ELSE)  $(INCPATH) -w 

$(OBJECTS): %.o: %.cpp $(HEADERS)
	$(CC) -c $< -o $@ -w $(INCPATH)
	
clean:
	-rm -rf *.o run
