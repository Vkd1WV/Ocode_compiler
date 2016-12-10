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

DEBUG_OPT:= #-DBLK_ADDR -DDBG_EMIT_IOP -DIOP_ADDR -DFLUSH_FILES

CFLAGS:= $(CWARNINGS) --std=c11 -I$(INCDIR) -O0 -I./ -L$(LIBDIR) -g $(DEBUG_OPT)
CXXFLAGS:= $(CPPWARNINGS) --std=c++14 -I$(INCDIR) -I./ -L$(LIBDIR) -g $(DEBUG_OPT)
LFLAGS:=#-d
LEX:= flex

################################## FILES #######################################

HEADERS:=global.h prog_data.h
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

main.o: main.c yuck.h $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ main.c

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

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

%.opp: %.cpp %.hpp $(HEADERS)
	$(CXX) $(CPPFLAGS) -c $<

################################## UTILITIES ###################################

CLEANFILES:= \
	*.o *.o *.opp occ \
	./tests/*.dbg ./tests/*.asm ./tests/*.pexe \
	global.c

VERYCLEANFILES:= scanner.c yuck.h yuck.c

.PHONEY: clean very-clean todolist test

clean:
	rm -f $(CLEANFILES)

very-clean:
	rm -f $(CLEANFILES) $(VERYCLEANFILES)

todolist:
	-@for file in $(ALLFILES:Makefile=); do fgrep -H -e TODO -e FIXME $$file; done; true

test: occ
	./tests/run


