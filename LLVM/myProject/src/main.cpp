#include <iostream>
#include "TokenStream.hpp"
using namespace std;

extern TokenStream *LexicalAnalysis(std::string input_filename);

int main(int argc, char** argv)
{
  if(argc > 1)
    LexicalAnalysis(argv[1]);
}
