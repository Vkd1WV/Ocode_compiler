# Omega make file

################################## FLAGS #######################################

SOURCEDIR:=$(HOME)/devel
INSTALLDIR:=$(HOME)/prg
LIBDIR:=$(INSTALLDIR)/lib
INCDIR:=$(INSTALLDIR)/include

CWARNINGS:=	-Wall -Wextra -pedantic \
	-Wmissing-prototypes -Wstrict-prototypes -Wmissing-declarations \
	-Wredundant-decls -Werror=implicit-function-declaration -Wnested-externs \
	-Wshadow -Wbad-function-cast \
	-Wcast-align \
	-Wdeclaration-after-statement -Werror=uninitialized \
	-Winline \
	-Wswitch-default -Wswitch-enum \
	-Wsuggest-attribute=pure -Wsuggest-attribute=const \
	-Wsuggest-attribute=noreturn -Wsuggest-attribute=format \
	-Wtrampolines -Wstack-protector \
	-Wwrite-strings -Wno-discarded-qualifiers \
	-Wc++-compat \
	 -Wconversion -Wdisabled-optimization \
	#-Wno-long-long -Wpadded

CPPWARNINGS:=	-Wall -Wextra -pedantic -Wfatal-errors \
	-Wmissing-declarations \
	-Wredundant-decls -Wshadow \
	-Wpointer-arith -Wcast-align \
	-Wuninitialized -Wmaybe-uninitialized \
	-Winline -Wno-long-long \
	-Wwrite-strings \
	-Wconversion


CFLAGS:= $(CWARNINGS) --std=c11 -I$(INCDIR) -L$(LIBDIR) -g -DDEBUG
CXXFLAGS:= $(CPPWARNINGS) --std=c++14 -I$(INCDIR) -L$(LIBDIR) -g -DDEBUG
LFLAGS:=#-d

################################## FILES #######################################

HEADERS:=compiler.h proto.h globals.h tokens.h types.h yuck.h
LIBS   :=-ldata

SRC    := \
	cmd_line.yuck globals.c main.c \
	scanner.l \
	parse_declarations.c parse_expressions.c parse_statements.c \
	intermediate.c icmd.cpp\
	opt.c \
	pexe.c arm.c x86.c

OBJECTS:= \
	yuck.o globals.o main.o \
	scanner.o \
	parse_expressions.o parse_statements.o parse_declarations.o \
	intermediate.o \
	opt.o \
	pexe.o x86.o #arm.o

ALLFILES:= $(SRC) $(HEADERS)

############################### PRODUCTIONS ####################################

occ: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)

scanner.c: scanner.l $(HEADERS)
	$(LEX) $(LFLAGS) -o $@ $<

yuck.c yuck.h: occ.yuck
	yuck gen -Hyuck.h -o yuck.c $<



main.o: main.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

# suppress warnings for third party slop
scanner.o: $(HEADERS) scanner.c
	$(CC) $(CFLAGS) -w -c -o $@ scanner.c
yuck.o: yuck.c yuck.h
	$(CC) $(CFLAGS) -w -c $<

%.o: %.c %.h $(HEADERS)
	$(CC) $(CFLAGS) -c $<

%.opp: %.cpp %.hpp $(HEADERS)
	$(CXX) $(CPPFLAGS) -c $<

################################## UTILITIES ###################################

CLEANFILES:= *.o *.opp occ ./tests/*.dbg ./tests/*.asm
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


