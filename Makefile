# Omega make file

CC:=gcc
LEX:=flex

CWARNINGS:=	-Wall -Wextra -pedantic \
	-Wmissing-prototypes -Wstrict-prototypes -Wmissing-declarations \
	-Wredundant-decls -Wnested-externs -Wshadow \
	-Wpointer-arith -Wcast-align \
	-Wuninitialized -Wmaybe-uninitialized \
	-Winline -Wno-long-long \
	-Wwrite-strings #-Wconversion


CFLAGS:= $(CWARNINGS) --std=c11 -g -DDEBUG
LFLAGS:=-d


SRC    :=scanner.l parse.c parse_expressions.c parse_statements.c
HEADERS:=tokens.h utility.h
OBJECTS:=scanner.o parse.o parse_expressions.o parse_statements.o
LIBS   :=

parse_headers:=tokens.h utility.h parse.h


scanner.o: scanner.c tokens.h utility.h
parse.o            : $(parse_headers) parse.c
parse_expressions.o: $(parse_headers) parse_expressions.c
parse_statements.o : $(parse_headers) parse_statements.c

scanner.c: scanner.l Makefile
	$(LEX) $(LFLAGS) -o $@ $<

compiler: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)


################################## UTILITIES ###################################
CLEANFILES:= $(OBJECTS) compiler scanner.c

.PHONEY: clean todolist
clean:
	rm -f $(CLEANFILES)


