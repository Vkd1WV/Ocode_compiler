# Omega make file

################################## FLAGS #######################################

SOURCEDIR:=$(HOME)/devel
INSTALLDIR:=$(HOME)/prg
LIBDIR:=$(INSTALLDIR)/lib
INCDIR:=$(INSTALLDIR)/include

CWARNINGS:=-Wall -Wextra -pedantic \
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
	-Wwrite-strings \
	-Wconversion -Wdisabled-optimization \
	# -Wc++-compat -Wpadded

CXXWARNINGS:=-Wall -Wextra -pedantic \
	-Wmissing-declarations \
	-Wredundant-decls -Wshadow \
	-Wpointer-arith -Wcast-align \
	-Wuninitialized -Wmaybe-uninitialized \
	-Winline -Wno-long-long \
	-Wwrite-strings \
	-Wconversion

DEBUG_OPT:= #-DBLK_ADDR -DDBG_EMIT_IOP -DIOP_ADDR -DFLUSH_FILES -DDBG_PARSE

CFLAGS:=  --std=c11   $(CWARNINGS) -I$(INCDIR) -I./ -L$(LIBDIR) -g
CXXFLAGS:=--std=c++14 $(CXXWARNINGS) -I$(INCDIR) -I./ -L$(LIBDIR) -g
LFLAGS:=#-d
LEX:= flex

################################## FILES #######################################

HEADERS:=prog_data.h errors.h my_types.h scanner.h token.h
LIBS   :=-ldata

PARSER:= \
	parse.cpp \
	parse_declarations.c parse_expressions.c parse_statements.c #emitters.c

SRC    := \
	Makefile cmd_line.yuck main.c \
	scanner.l scanner.cpp \
	$(PARSER) \
	opt.c \
	gen-pexe.c gen-arm.c gen-x86.c

AUTOFILES:=lex.yy.c yuck.h yuck.c

OBJECTS:= \
	yuck.o global.o main.o \
	lex.yy.o scanner.o parse.o \
	opt.o prog_data.o \
	gen-pexe.o gen-x86.o #gen-arm.o

ALLFILES:= $(SRC) $(HEADERS)



.PHONEY: all dist dev-occ
all: occ scantest
dist: CFLAGS += $(CWARNINGS)
dist: $(AUTOFILES)
dev-occ: CFLAGS +=  $(DEBUG_OPT)
dev-occ: CXXFLAGS +=  $(DEBUG_OPT)
dev-occ: occ

############################### PRODUCTIONS ####################################

scantest: LFLAGS += -d
scantest: CFLAGS += $(CWARNINGS) $(DEBUG_OPT)
scantest: scantest.c scanner.o lex.yy.o global.o scanner.h
	$(CXX) $(CXXFLAGS) -o $@ scantest.c scanner.o global.o lex.yy.o


occ: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(LIBS)

parse.o: $(PARSER) scanner.h prog_data.h
	$(CXX) $(CXXFLAGS) -Wno-switch-enum -c -o $@ parse.cpp

scope.o: scope.cpp scope.h prog_data.h
scanner.o: scanner.cpp scanner.h lex.h
main.o: main.cpp yuck.h errors.h my_types.h prog_data.h proto.h scanner.h
global.o: global.cpp prog_data.h errors.h token.h lex.h
opt.o: opt.cpp prog_data.h proto.h


# suppress warnings for third party slop
lex.yy.o: lex.yy.c lex.h 
	$(CXX) $(CXXFLAGS) -fpermissive -Wno-conversion -Wno-switch-default -Wno-sign-compare -c lex.yy.c
yuck.o: yuck.c yuck.h
	$(CC) $(CFLAGS) -Wno-switch-enum -Wno-sign-conversion -c $<


# Automatically generated files
lex.yy.c: lex.l
	$(LEX) $(LFLAGS) $<
yuck.c yuck.h: occ.yuck
	yuck gen -Hyuck.h -o yuck.c $<


# General Rules
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

%.o: %.cpp %.h $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $<

################################## UTILITIES ###################################

CLEANFILES:= \
	*.o *.o *.opp occ scantest \
	./tests/*.dbg ./tests/*.asm ./tests/*.pexe \
	*.output *.tab.c

.PHONEY: clean very-clean todolist test

clean:
	rm -f $(CLEANFILES)

very-clean:
	rm -f $(CLEANFILES) $(AUTOFILES)

todolist:
	-@for file in $(ALLFILES:Makefile=); do fgrep -H -e TODO -e FIXME $$file; done; true

test: occ
	./tests/run


