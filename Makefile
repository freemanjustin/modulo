###################################################################
#
#  hopfield makefile
#
#  freeman.justin@gmail.com
#
##################################################################

CSRC=	./src
OBJ=	$(CSRC)/brain_lib.o \
	$(CSRC)/graphics.o\
	$(CSRC)/main.o \
	$(CSRC)/nrutil.o \
	$(CSRC)/rand.o \
	$(CSRC)/readparams.o \
	$(CSRC)/sort.o \
	$(CSRC)/utils.o
		

# compliler flags
INC=	-I./ -I./include

CFLAGS=	-O3 -finline-functions -funroll-loops -Wall

CC=	gcc $(CFLAGS) $(INC)

# Libraries for linking
LFLAGS=	-lm -lSDL2 -framework OpenGL 
#-framework Cocoa

# Name of executable goes here:

EXEC=	./bin/hopfield	

$(EXEC):$(OBJ)
	$(CC) -o $(EXEC) $(OBJ) $(LFLAGS)

clean:
	rm $(OBJ)
	rm $(EXEC)
