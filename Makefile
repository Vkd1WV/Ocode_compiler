# Omega make file
SOURCEDIR:=$(HOME)/devel
INSTALLDIR:=$(HOME)/prg
LIBDIR:=$(INSTALLDIR)/lib
INCDIR:=$(INSTALLDIR)/include

#LEX:=flex

CWARNINGS:=	-Wall -Wextra -pedantic \
	-Wmissing-prototypes -Wstrict-prototypes -Wmissing-declarations \
	-Wredundant-decls -Wnested-externs -Wshadow \
	-Wpointer-arith -Wcast-align \
	-Wuninitialized -Wmaybe-uninitialized \
	-Winline -Wno-long-long \
	-Wwrite-strings \
	-Wno-discarded-qualifiers #-Wconversion


CFLAGS:= $(CWARNINGS) --std=c11 -I$(INCDIR) -L$(LIBDIR) -g -DDEBUG
LFLAGS:=#-d


SRC    :=scanner.l parse.c parse_declarations.c parse_expressions.c parse_statements.c globals.c functions.c intermediate.c #x86-64.c arm.c

HEADERS:=compiler.h functions.h globals.h tokens.h types.h

OBJECTS:= \
	scanner.o globals.o main.o \
	parse_expressions.o parse_statements.o parse_declarations.o \
	intermediate.o \
	#x86-64.o arm.o

LIBS   :=-ldata

ALLFILES:= $(SRC) $(HEADERS)

scanner.c: scanner.l Makefile $(HEADERS)
	$(LEX) $(LFLAGS) -o $@ $<

scanner.o: $(HEADERS) scanner.c
	$(CC) $(CFLAGS) -Wno-unused-function -c -o $@ scanner.c

occ: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

################################## UTILITIES ###################################
CLEANFILES:= $(OBJECTS) occ scanner.c ./test/*.dbg

.PHONEY: clean todolist test
clean:
	rm -f $(CLEANFILES)

todolist:
	-@for file in $(ALLFILES:Makefile=); do fgrep -H -e TODO -e FIXME $$file; done; true

test: compiler
	./test.sh


