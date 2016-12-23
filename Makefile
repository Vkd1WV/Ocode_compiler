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
	 -Wc++-compat -Wpadded

CXXWARNINGS:=-Wall -Wextra -pedantic \
	-Wmissing-declarations -Werror=implicit-function-declaration \
	-Wredundant-decls -Wshadow \
	-Wpointer-arith -Wcast-align \
	-Wuninitialized -Wmaybe-uninitialized -Werror=uninitialized \
	-Winline -Wno-long-long \
	-Wswitch \
	-Wsuggest-attribute=pure -Wsuggest-attribute=const \
	-Wsuggest-attribute=noreturn -Wsuggest-attribute=format \
	-Wtrampolines -Wstack-protector \
	-Wwrite-strings \
	-Wconversion -Wdisabled-optimization

DEBUG_OPT:=-DDBG_PARSE -DDBG_SCAN \
#-DBLK_ADDR -DDBG_EMIT_IOP -DIOP_ADDR -DFLUSH_FILES

CFLAGS:=  --std=c11   $(CWARNINGS) -I$(INCDIR) -I./ -L$(LIBDIR) -g
CXXFLAGS:=--std=c++14 $(CXXWARNINGS) $(DEBUG_OPT) -I$(INCDIR) -I./ -L$(LIBDIR) -g
LFLAGS:=#-d
LEX:= flex

################################## FILES #######################################

HEADERS:=prog_data.h errors.h my_types.h proto.h scanner.h token.h
LIBS   :=-ldata

PARSER:= \
	parse.cpp \
	parse_declarations.cpp parse_expressions.cpp parse_statements.cpp

SRC    := \
	Makefile cmd_line.yuck main.c \
	scanner.l scanner.cpp \
	$(PARSER) \
	opt.c \
	gen-pexe.c gen-arm.c gen-x86.c

AUTOFILES:=lex.yy.c yuck.h yuck.c

OCC_OBJECTS:= \
	yuck.o global.o main.o \
	lex.yy.o scanner.o parse.o \
	opt.o prog_data.o scope.o \
	gen-pexe.o gen-x86.o #gen-arm.o

SCAN_OBJECTS:=global.o lex.yy.o scanner.o prog_data.o scope.o scantest.o

ALLFILES:= $(SRC) $(HEADERS)



.PHONEY: all dist dev-occ
all: occ scantest
dist: CFLAGS += $(CWARNINGS)
dist: $(AUTOFILES)
dev-occ: CFLAGS +=  $(DEBUG_OPT)
dev-occ: CXXFLAGS +=  $(DEBUG_OPT)
dev-occ: occ

############################### PRODUCTIONS ####################################

scantest: CXXFLAGS += $(DEBUG_OPT)
scantest: $(SCAN_OBJECTS) scanner.h prog_data.h scope.h token.h errors.h
	$(CXX) $(CXXFLAGS) -o $@ $(SCAN_OBJECTS) $(LIBS)


occ: $(OCC_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OCC_OBJECTS) $(LIBS)

parse.o: $(PARSER) scanner.h prog_data.h errors.h scope.h proto.h
	$(CXX) $(CXXFLAGS) -Wno-switch -c -o $@ parse.cpp

scanner.o: scanner.cpp scanner.h   errors.h scope.h lex.h
scope.o: scope.cpp     prog_data.h errors.h scope.h
main.o: main.cpp       prog_data.h errors.h proto.h scanner.h yuck.h my_types.h
global.o: global.cpp   prog_data.h errors.h token.h lex.h
opt.o: opt.cpp         prog_data.h errors.h proto.h


# suppress warnings for third party slop
lex.yy.o: lex.yy.c lex.h 
	$(CXX) $(CXXFLAGS) -fpermissive -Wno-conversion -Wno-switch-default -Wno-sign-compare -c lex.yy.c
yuck.o: yuck.c yuck.h
	$(CC) $(CFLAGS) -Wno-padded -Wno-c++-compat -Wno-switch-enum -Wno-sign-conversion -c $<


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


