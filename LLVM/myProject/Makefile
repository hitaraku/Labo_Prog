CXX=g++
CXX_FLAGS= -c -g -Iheader
OBJECTS=TokenStream.o lexer.o main.o

main: $(OBJECTS)
	$(CXX) -g $(OBJECTS) -o $@
	rm -f *.o

# $@: The file name of the target of the rule.
# $<: The name of the first prerequisite.
%.o: src/%.cpp
	$(CXX) $(CXX_FLAGS) $< -o $@

clean:
	rm -f *.o
	rm main
