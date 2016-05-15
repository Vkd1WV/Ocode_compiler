# Omega make file
SOURCEDIR:=$(HOME)/devel
INSTALLDIR:=$(HOME)/prg
LIBDIR:=$(INSTALLDIR)/lib
INCDIR:=$(INSTALLDIR)/include

CC:=gcc
LEX:=flex

CWARNINGS:=	-Wall -Wextra -pedantic \
	-Wmissing-prototypes -Wstrict-prototypes -Wmissing-declarations \
	-Wredundant-decls -Wnested-externs -Wshadow \
	-Wpointer-arith -Wcast-align \
	-Wuninitialized -Wmaybe-uninitialized \
	-Winline -Wno-long-long \
	-Wwrite-strings #-Wconversion


CFLAGS:= $(CWARNINGS) --std=c11 -I$(INCDIR) -L$(LIBDIR) -g -DDEBUG
LFLAGS:=-d


SRC    :=scanner.l parse.c parse_expressions.c parse_statements.c globals.c
HEADERS:=compiler.h functions.h globals.h tokens.h
OBJECTS:=main.o scanner.o parse_expressions.o parse_statements.o globals.o
LIBS   :=-ldata

%.o: %.c $(HEADERS)

scanner.c: scanner.l Makefile
	$(LEX) $(LFLAGS) -o $@ $<

compiler: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)


################################## UTILITIES ###################################
CLEANFILES:= $(OBJECTS) compiler scanner.c

.PHONEY: clean todolist
clean:
	rm -f $(CLEANFILES)


