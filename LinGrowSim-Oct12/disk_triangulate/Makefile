# the name of the executable to be linked
TARGET = triangulate

# the name/location of the C-compiler
CC = gcc

INCDIR = include

OPTIMIZE = -O3
CFLAGS =  $(OPTIMIZE) -I$(INCDIR)/ -I/usr/include/
LDFLAGS  = -lm

TRI_FILES   = main init neighbours place_nodes

# append .o and .c to the names in TRI_FILES
TRI_OBJ = $(TRI_FILES:=.o)
TRI_SRC = $(TRI_FILES:=.c)


newstuff: $(TARGET)

# how to compile a .c file to produce a .o file.
.c.o:
	@echo Compiling $@
	@$(CC) -o $@ $< -c $(CFLAGS)

#how to build TARGET
$(TARGET): $(TRI_OBJ)
	 @echo Linking $(TARGET)
	 @$(CC) -o $(TARGET) $(TRI_OBJ) $(LDFLAGS)


#main.o should also be recompiled when globals.h changes!
main.o: main.c $(INCDIR)/globals.h
	@echo Compiling $@
	@$(CC) -o $@ $< -c $(CFLAGS)

clean:
	@echo Removing all object files for $(TARGET)
	@rm -f $(TRI_OBJ)

new:
	@echo Removing all object-files for $(TARGET)
	@rm -f $(TRI_OBJ)
	@echo Rebuilding all.
	@(make)

wc:
	@echo Word Count:
	@echo "    Lines   Words  Characters "
	@wc $(TRI_SRC)  $(INCDIR)/*.h | grep total


#A program to generate input for gnuplot to
#plot the triangulation
#Use
# make plottri
#get the executable called by
# plottri

plottri: plottriangulation.c
	$(CC) -o plottri plottriangulation.c -lm

#A program to give counts of nodes versus
#number of nodes around circumference
#(simple stuff)
table: table.c
	$(CC) -o table table.c 
