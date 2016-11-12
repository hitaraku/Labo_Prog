CXX = g++
CLANG = clang
CFLAGS = -g -std=c++0x
OBJECTS = main.o TokenStream.o
LLVMFLG = `llvm-config --cxxflags`

main: $(OBJECTS)
	$(CXX) $(LLVMFLG) $(OBJECTS) -o $@

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o
	rm -rf main