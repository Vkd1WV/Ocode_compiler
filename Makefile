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
	-Wconversion -Wdisabled-optimization \
	# -Wc++-compat -Wpadded

CPPWARNINGS:=	-Wall -Wextra -pedantic -Wfatal-errors \
	-Wmissing-declarations \
	-Wredundant-decls -Wshadow \
	-Wpointer-arith -Wcast-align \
	-Wuninitialized -Wmaybe-uninitialized \
	-Winline -Wno-long-long \
	-Wwrite-strings \
	-Wconversion


CFLAGS:= $(CWARNINGS) --std=c11 -I$(INCDIR) -O0 -I./ -L$(LIBDIR) -g -DDEBUG
CXXFLAGS:= $(CPPWARNINGS) --std=c++14 -I$(INCDIR) -I./ -L$(LIBDIR) -g -DDEBUG
LFLAGS:=#-d

################################## FILES #######################################

HEADERS:=global.h yuck.h prog_data.h
LIBS   :=-ldata

PARSER:= \
	parse.c \
	parse_declarations.c parse_expressions.c parse_statements.c emitters.c

SRC    := \
	Makefile cmd_line.yuck main.c \
	scanner.l \
	$(PARSER) \
	opt.c \
	gen-pexe.c gen-arm.c gen-x86.c

OBJECTS:= \
	yuck.o global.o main.o \
	scanner.o parse.o \
	opt.o \
	gen-pexe.o gen-x86.o #gen-arm.o

ALLFILES:= $(SRC) $(HEADERS)

############################### PRODUCTIONS ####################################

occ: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)

parse.o: $(PARSER) $(HEADERS) scanner.h
	$(CC) $(CFLAGS) -Wno-switch-enum -c -o $@ parse.c

global.c: Makefile
	echo "#define _GLOBALS_C\n#include \"global.h\"" > $@

scanner.c: scanner.l $(HEADERS)
	$(LEX) $(LFLAGS) -o $@ $<

yuck.c yuck.h: occ.yuck
	yuck gen -Hyuck.h -o yuck.c $<

# suppress warnings for third party slop
scanner.o: $(HEADERS) scanner.c scanner.h
	$(CC) $(CFLAGS) -w -c -o $@ scanner.c
yuck.o: yuck.c yuck.h
	$(CC) $(CFLAGS) -w -c $<

%.o: %.c %.h $(HEADERS)
	$(CC) $(CFLAGS) -c $<

%.opp: %.cpp %.hpp $(HEADERS)
	$(CXX) $(CPPFLAGS) -c $<

################################## UTILITIES ###################################

CLEANFILES:= \
	*.o *.o *.opp occ \
	./tests/*.dbg ./tests/*.asm ./tests/*.pexe \
	global.c scanner.c yuck.h yuck.c

.PHONEY: clean todolist test

clean:
	rm -f $(CLEANFILES)

todolist:
	-@for file in $(ALLFILES:Makefile=); do fgrep -H -e TODO -e FIXME $$file; done; true

test: occ
	./tests/run


