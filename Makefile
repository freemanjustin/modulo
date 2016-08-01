###################################################################
#
#  maX makefile
#
#  issues -> j.freeman@csse.monash.edu
#
##################################################################


OBJ=	./brain_lib.o \
		./graphics.o\
		./main.o \
		./nrutil.o \
		./rand.o \
		./readparams.o \
		./sort.o \
		./utils.o
		

# compliler flags
#INC=    -I./ -I./include -I/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers/ -I/System/Library/Frameworks/GLUT.framework/Versions/A/Headers/
INC=	-I./ -I./include

CFLAGS=	-O3 -finline-functions -funroll-loops -Wall

CC=	gcc $(CFLAGS) $(INC)

# Libraries for linking

#LFLAGS=	-lm -L/sw/lib -lSDLmain -lSDL -framework Cocoa -framework OpenGL 
LFLAGS=	-lm -lSDL2 -framework OpenGL -framework Cocoa

# Name of executable goes here:

EXEC=	./bin/modulo	

$(EXEC):$(OBJ)
	$(CC) -o $(EXEC) $(OBJ) $(LFLAGS)

clean:
	rm $(OBJ)
	rm $(EXEC)
