# Omega make file

################################## FLAGS #######################################

SOURCEDIR:=$(HOME)/devel
INSTALLDIR:=$(HOME)/prg
LIBDIR:=$(INSTALLDIR)/lib
INCDIR:=$(INSTALLDIR)/include

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

################################## FILES #######################################

HEADERS:=compiler.h functions.h globals.h tokens.h types.h yuck.h
LIBS   :=-ldata

SRC    := \
	cmd_line.yuck globals.c main.c \
	scanner.l \
	parse_declarations.c parse_expressions.c parse_statements.c \
	intermediate.c \
	pexe.c arm.c x86-64.c

OBJECTS:= \
	yuck.o globals.o main.o \
	scanner.o \
	parse_expressions.o parse_statements.o parse_declarations.o \
	intermediate.o \
	pexe.o #x86-64.o arm.o

ALLFILES:= $(SRC) $(HEADERS)

############################### PRODUCTIONS ####################################

occ: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)

scanner.c: scanner.l $(HEADERS)
	$(LEX) $(LFLAGS) -o $@ $<

yuck.c yuck.h: occ.yuck
	yuck gen -Hyuck.h -o yuck.c $<

scanner.o: $(HEADERS) scanner.c
	$(CC) $(CFLAGS) -Wno-unused-function -c -o $@ scanner.c

main.o: main.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

%.o: %.c %.h $(HEADERS)
	$(CC) $(CFLAGS) -c $<

################################## UTILITIES ###################################

CLEANFILES:= $(OBJECTS) occ ./tests/*.dbg
VERYCLEANFILES:= $(CLEANFILES) scanner.c yuck.h yuck.c

.PHONEY: clean todolist test very-clean
very-clean:
	rm -f $(VERYCLEANFILES)

clean:
	rm -f $(CLEANFILES)

todolist:
	-@for file in $(ALLFILES:Makefile=); do fgrep -H -e TODO -e FIXME $$file; done; true

test: occ
	./tests/run


