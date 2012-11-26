TARGET = growth_srd

# the name/location of the C-compiler
CC = gcc

INCDIR = ./include

OPTIMIZE = -O3
CFLAGS =  -I$(INCDIR)/ -I/usr/include/
LDFLAGS  = -lm

SRD_FILES = dump growth init_distribution			\
	init_surface loop main memory random	\
	reaction_diffusion read_parameters refine setup_matrix	\
	solve_system triangle


# append .o and .c to the names in SRD_FILES
SRD_OBJ = $(SRD_FILES:=.o)
SRD_SRC = $(SRD_FILES:=.c)


newstuff: $(TARGET)

# how to compile a .c file to produce a .o file.
.c.o:
	@echo Compiling $@
	@$(CC) -o $@ $< -c $(CFLAGS)

#how to build TARGET
$(TARGET): $(SRD_OBJ)
	 @echo Linking $(TARGET)
	 $(CC) -o $(TARGET) $(SRD_OBJ) $(LDFLAGS)

#main.o should also be recompiled when globals.h changes!
main.o: main.c $(INCDIR)/globals.h
	@echo Compiling $@
	@$(CC) -o $@ $< -c $(CFLAGS)

clean:
	@echo Removing all object files for $(TARGET)
	@rm -f $(SRD_OBJ)

new:
	@echo Removing all object-files for $(TARGET)
	@rm -f $(SRD_OBJ)
	@echo Rebuilding all.
	@(make)

wc:
	@echo Word Count:
	@echo "    Lines   Words  Characters "
	@wc $(SRD_SRC)  $(INCDIR)/*.h | grep total
