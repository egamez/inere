#
CC		:= gcc
DBG		:= -g -O2
XTRACCFLAGS	:= -fPIC -pipe -funsigned-char -ansi -pedantic -pedantic-errors
INCLUDES	:= -I../include
LIBS		:= -L../lib/.libs -linere

WARNINGS	:= -Wextra -Wall -W
CCFLAGS		:= $(DBG) $(WARNINGS) $(XTRACCFLAGS) $(INCLUDES)

RFC_EXE		:= obten_rfc
RFC_SOURCE	:= obten_rfc.c
RFC_OBJ		:= obten_rfc.o

HOMO_EXE	:= obten_homonimia
HOMO_SOURCE	:= obten_homonimia.c
HOMO_OBJ	:= obten_homonimia.o

DIG_EXE		:= obten_digito
DIG_SOURCE	:= obten_digito.c
DIG_OBJ		:= obten_digito.o

VER_EXE		:= verifica_rfc
VER_SRC		:= verifica_rfc.c
VER_OBJ		:= verifica_rfc.o

# Targets
.SUFFIXES:	.c .o
.PHONY:		clean

all: $(RFC_EXE) $(HOMO_EXE) $(DIG_EXE) $(VER_EXE)

$(RFC_EXE): $(RFC_OBJ)
	$(CC) $(DBG) -o $@ $(RFC_OBJ) $(LIBS)

$(HOMO_EXE): $(HOMO_OBJ)
	$(CC) $(DBG) -o $@ $(HOMO_OBJ) $(LIBS)

$(DIG_EXE): $(DIG_OBJ)
	$(CC) $(DBG) -o $@ $(DIG_OBJ) $(LIBS)

$(VER_EXE): $(VER_OBJ)
	$(CC) $(DBG) -o $@ $(VER_OBJ) $(LIBS)

clean:
	rm -f $(RFC_EXE) $(HOMO_EXE) $(DIG_EXE) $(RFC_OBJ) $(HOMO_OBJ) $(DIG_OBJ) $(VER_EXE) $(VER_OBJ)

# Rules
$(RFC_OBJ): $(RFC_SRC)
	@echo compiling $< ...
	$(CC) $(CCFLAGS) -c $< -o $@

$(HOMO_OBJ): $(HOMO_SRC)
	@echo compiling $< ...
	$(CC) $(CCFLAGS) -c $< -o $@

$(DIG_OBJ): $(DIG_SRC)
	@echo compiling $< ...
	$(CC) $(CCFLAGS) -c $< -o $@

$(VER_OBJ): $(VER_SRC)
	@echo compiling $< ...
	$(CC) $(CCFLAGS) -c $< -o $@
