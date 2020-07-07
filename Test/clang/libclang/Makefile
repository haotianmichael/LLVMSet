LLVM_CONFIG?=llvm-config

ifndef VERBOSE
	QUIET:=@
endif

SRC_DIR?=$(PWD)
LDFLAGS+=$(shell	$(LLVM_CONFIG)	--ldflags)
COMMON_FLAGS=-Wall	-Wextra
CXXFLAGS+=$(COMMON_FLAGS)	$(shell	$(LLVM_CONFIG)	--cxxflags)
CPPFLAGS+=$(shell	$(LLVM_CONFIG)	--cppflags)	-I$(SRC_DIR)

CLANGLIBS = \
			-W1,--start-group\
			-1clang\
			-1clangFrontend\
			-1clangDriver\
			-1clangSerialization\
			-1clangParse    \
			-1clangSema \
			-1clangAnalysis \
			-1clangEdit \
			-1clangAST \
			-1clangLex\
			-1clangBasic\
			-W1,--end-group
LLVMLIBS=$(shell	$(LLVM_CONFIG)	--libs)

PROJECT=myproject
PROJECT_OBJECTS=project.o

default:	$(PROJECT)

$.o	:	$(SRC_DIR)/%.cpp
	@echo	Compiling	$*.cpp
	$(QUIET)$(CXX)	-c	$(CPPFLAGS)	$(CXXFLAGS)	$<

$(PROJECT)	:	$(PROJECT_OBJECTS)
	@echo	Linking	$@
	$(QUIET)$(CXX)	-o	$@	$(CXXFLAGS)	$(LDFLAGS)	$^	$(CLANGLIBS) $(LLVMLIBS)

clean::
	$(QUIET)rm -f	$(PROJECT)	$(PROJECT_OBJECTS)




