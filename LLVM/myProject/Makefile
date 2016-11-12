CXX=g++
CLANGXX=clang++
PROJECT_DIR=.

SRC_DIR = $(PROJECT_DIR)/src
INC_DIR = $(PROJECT_DIR)/inc
OBJ_DIR = $(PROJECT_DIR)/obj
BIN_DIR = $(PROJECT_DIR)/bin
LIB_DIR = $(PROJECT_DIR)/lib

OBJECTS=TokenStream.o main.o Parser.o AST.o

INC_FLAGS= -c -g -Iinc -I/usr/include/x86_64-linux-gnu/c++/4.8 

LLVM_CONFIG = llvm-config
LLVM_FLAGS = --cxxflags --ldflags --libs


main: $(OBJECTS)
	$(CLANGXX) -g $(OBJECTS:%=$(OBJ_DIR)/%) -o $@

# $@: The file name of the target of the rule.
# $<: The name of the first prerequisite.
%.o: src/%.cpp
	$(CXX) -c -g $< $(INC_FLAGS) `$(LLVM_CONFIG) $(LLVM_FLAGS)` -o $(OBJ_DIR)/$@

clean:
	rm -f *.o
	rm main
