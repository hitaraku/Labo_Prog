CXX = g++
CLANG = clang++
CFLAGS = -g --std=c++0x
OBJECTS = main.o TokenStream.o CodeGen.o Parser.o AST.o
LLVMFLG = llvm-config
CONFIG = --cxxflags --ldflags --libs

main: $(OBJECTS)
	$(CLANG) -g $(OBJECTS) `$(LLVMFLG) $(CONFIG)` -ldl -o $@

%.o: %.cpp
	$(CXX) $(CFLAGS) `$(LLVMFLG) $(CONFIG)` -c $< -o $@

clean:
	rm -rf *.o
	rm -rf main