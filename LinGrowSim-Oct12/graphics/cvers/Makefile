TARGET = ./displayer

CC = gcc

SHELL = /bin/sh

LIBTOOL = $(SHELL) /usr/bin/libtool

BRUSSELDIR = ./
INCDIR = $(BRUSSELDIR)include/
INCDIR2 = /usr/X11R6/include/GL/
#DEBUG  =  -g
OPTIMIZE = -O2


CFLAGS = -I$(INCDIR) -I$(INCDIR2) -g -O2 -Wall -fomit-frame-pointer -ffast-math -fexpensive-optimizations -fstrict-aliasing -malign-loops=2 -malign-jumps=2 -malign-functions=2 -D_REENTRANT -DPTHREADS

LINK = $(LIBTOOL) --mode=link $(CC)  $(CFLAGS)  -o $@

LDFLAGS  =  -L/usr/lib -lm -lGL -lGLU -lglut  


BINDIR = $(BRUSSELDIR)
SRCDIR = $(BRUSSELDIR)
DISPDEP   = /usr/Mesa-5.1/lib/libGLU.so /usr/Mesa-5.1/lib/libGL.so 

OBJ   = $(BINDIR)main.o $(BINDIR)display.o $(BINDIR)memory.o \
          $(BINDIR)dump.o $(BINDIR)triangle.o 


SRC   =	

all: $(TARGET)

$(BINDIR)main.o : $(SRCDIR)main.c 
	@echo Compiling main.c
	$(CC) -o $@ $(SRCDIR)main.c -c  $(CFLAGS)

$(BINDIR)dump.o:$(SRCDIR)dump.c
	@echo Compiling dump.c
	$(CC) -o $@ $(SRCDIR)dump.c -c $(CFLAGS)

$(BINDIR)display.o : $(SRCDIR)display.c 
	@echo Compiling display.c
	$(CC) -o $@ $(SRCDIR)display.c -c $(CFLAGS)

$(BINDIR)memory.o : $(SRCDIR)memory.c 
	@echo Compiling memory.c
	$(CC) -o $@ $(SRCDIR)memory.c -c $(CFLAGS)

$(BINDIR)triangle.o : $(SRCDIR)triangle.c 
	@echo Compiling triangle.c
	$(CC) -o $@ $(SRCDIR)triangle.c -c $(CFLAGS)

# how to compile a .c file to produce a .o file.
#.c.o : 
#	@echo Compiling $@
#	$(CC) $(CFLAGS) -c $*.c 
          

#how to build TARGET

$(TARGET): $(OBJ) $(DISPDEP)
	 @echo Linking $(TARGET)
#	 $(CC) -o $(TARGET) $(OBJ) $(LDFLAGS) 
	$(LINK)  $(OBJ) /usr/Mesa-5.1/lib/libglut.so $(DISPDEP) -lm

clean:
	@echo Cleared $(BINDIR).
	@rm -f $(BINDIR)*.o

new: 	
	@echo Removing all object-files.
	@echo $(OBJ)
	@rm -f $(OBJ) 
	@echo Rebuilding all.
	@(make)

wc:
	@echo Word Count:
	@wc $(SRC)  $(INCDIR)*.h | grep total

