LLVM_CONFIG?=llvm-config

ifndef  VERBOSE
    QUIET:=@
endif

SRC_DIR?=$(PWD)
LDFLAGS+=$(shell    $(LLVM_CONFIG)  --ldflags)
COMMON_FLAGS=-Wall  -Wextra
CXXFLAGS+=$(COMMON_FLAGS)   $(LLVM_CONFIG) --cxxflags
CPPFLAGS+=$(shell   $(LLVM_CONFIG) --cppflags) -I$(SRC_DIR)


HELLO=helloworld
HELLO_OBJECTS=hello.o
default:	$(HELLO)

%.o:	$(SRC_DIR)/%.cpp
	@echo Compiling	$*.cpp
	$(QUIET)$(CXX)	-c	$(CPPFLAGS)	$(CXXFLAGS)	$<


$(HELLO)	:	$(HELLO_OBJECTS)
	@echo	Linking 	$@
	$(QUIT)$(CXX)	-o $@	$(CXXFLAGS)	$(LDFLAGS)	$^	`$(LLVM_CONFIG)

--libs bitreader core support

clean::
	$(QUIT)rm 	-f $(HELLO)	$(HELLO_OBJECTS)
