#include <iostream>
#include "Parser.hpp"
using namespace std;

extern TokenStream *LexicalAnalysis(std::string input_filename);

int main(int argc, char** argv)
{
  if(argc > 1) {
    Parser *p = new Parser(argv[1]);
  }
}
