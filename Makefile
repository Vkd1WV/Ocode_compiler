# Omega make file

################################## FILES #######################################

LIBS   :=-ldata -lmsg

### DIRECTORIES

WORKDIR   :=./work
SRCDIR    :=./src
INSTALLDIR:=$(HOME)/prg
LIBDIR    :=$(INSTALLDIR)/lib/util
INCDIR    :=$(INSTALLDIR)/include

### SOURCES

# Presource files
lex_source:=lex.l
yuck_source:=occ.yuck

# Intermediate Source names
lex_cpp:=lex.cpp
yuck_c:=yuck.c
yuck_h:=yuck.h

# Source file names
headers:=$(wildcard $(SRCDIR)/*.h)
cpp_sources:=$(wildcard $(SRCDIR)/*.cpp)

parse_sources:= parse.cpp parse_declarations.cpp

#Prefix sources
lex_cpp      :=$(addprefix $(SRCDIR)/, $(lex_cpp))
yuck_c       :=$(addprefix $(SRCDIR)/, $(yuck_c))
yuck_h       :=$(addprefix $(SRCDIR)/, $(yuck_h))
parse_sources:=$(addprefix $(SRCDIR)/, $(parse_sources))

# intermediate source files
autofiles:=$(lex_cpp) $(yuck_c) $(yuck_h)

interface:= $(headers) $(yuck_h)
c_sources:= $(yuck_c)

### OBJECTS

CPP_OBJECTS:= \
	scantest.o \
	prog_data.o \
	iq.o \
	scanner.o \
	scope.o \
	opt.o \
	gen-pexe.o gen-x86.o \
	global.o main.o

yuck_o:=yuck.o
lex_o :=lex.o
parse_o:=parse.o

OCC_OBJECTS:= \
	lex.o scanner.o \
	prog_data.o \
	scope.o parse.o \
	opt.o \
	gen-pexe.o gen-x86.o \
	yuck.o global.o main.o

SCAN_OBJECTS:=global.o lex.o scanner.o scope.o prog_data.o scantest.o

#prefix objects
OCC_OBJECTS :=$(addprefix $(WORKDIR)/, $(OCC_OBJECTS) )
SCAN_OBJECTS:=$(addprefix $(WORKDIR)/, $(SCAN_OBJECTS))
CPP_OBJECTS :=$(addprefix $(WORKDIR)/, $(CPP_OBJECTS))
yuck_o      :=$(addprefix $(WORKDIR)/, $(yuck_o))
lex_o       :=$(addprefix $(WORKDIR)/, $(lex_o))
parse_o     :=$(addprefix $(WORKDIR)/, $(parse_o))

source_files:= Makefile $(headers) $(cpp_sources) $(lex_source) $(yuck_source)

################################## FLAGS #######################################


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

DEBUG_OPT:=#-DDBG_PARSE -DDBG_SCAN \
#-DBLK_ADDR -DDBG_EMIT_IOP -DIOP_ADDR -DFLUSH_FILES

CFLAGS:=  --std=c11   -g $(CWARNINGS) -I$(INCDIR) -I./ -L$(LIBDIR)
CXXFLAGS:=--std=c++14 -g $(CXXWARNINGS) -I$(INCDIR) -I./ -L$(LIBDIR)
LFLAGS:=#-d
LEX:= flex

################################# TARGETS ######################################

.PHONEY: all dist print debug
all: scantest occ
dist: clean $(autofiles)

print:
	@echo "headers:" $(headers)
	@echo "cpp_sources:" $(cpp_sources)
	@echo "scan objects:" $(SCAN_OBJECTS)
	@echo "occ objects:" $(OCC_OBJECTS)

debug: CXXFLAGS += $(DEBUG_OPT)
debug: scantest omega

############################### PRODUCTIONS ####################################

scantest: $(SCAN_OBJECTS) $(interface)
	$(CXX) $(CXXFLAGS) -O0 -o $@ $(SCAN_OBJECTS) $(LIBS)

occ: $(OCC_OBJECTS) $(interface)
	$(CXX) $(CXXFLAGS) -o $@ $(OCC_OBJECTS) $(LIBS)

# objects
$(CPP_OBJECTS): $(WORKDIR)/%.o: $(SRCDIR)/%.cpp $(interface) | $(WORKDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(parse_o): $(parse_sources) | $(WORKDIR)
	$(CXX) $(CXXFLAGS) -Wno-switch -c -o $@ $<

# suppress warnings for third party slop
$(lex_o): $(lex_cpp) $(inteface) | $(WORKDIR)
	$(CXX) $(CXXFLAGS) -fpermissive -Wno-conversion -Wno-switch-default -Wno-sign-compare -c -o $@ $<
$(yuck_o): $(yuck_c) $(yuck_h) | $(WORKDIR)
	$(CC) $(CFLAGS) -Wno-padded -Wno-c++-compat -Wno-switch-enum -Wno-sign-conversion -c -o $@ $<

# Automatically generated files
$(lex_cpp): $(lex_source)
	$(LEX) $(LFLAGS) -o $@ $<
$(yuck_c) $(yuck_h): $(yuck_source)
	yuck gen -H$(yuck_h) -o $(yuck_c) $<

# working directory
$(WORKDIR):
	mkdir -p $@

################################## UTILITIES ###################################

CLEANFILES:= \
	*.o *.opp occ scantest omega \
	./tests/*.dbg ./tests/*.asm ./tests/*.pexe \
	*.output *.tab.c

.PHONEY: clean very-clean todolist test

clean:
	rm -f $(CLEANFILES)
	rm -fr $(WORKDIR)

very-clean: clean
	rm -f $(autofiles)

todolist:
	-@for file in $(ALLFILES:Makefile=); do fgrep -H -e TODO -e FIXME $$file; done; true

test: occ
	./tests/run


