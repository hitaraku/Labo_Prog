#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char** argv)
{
  LaxycalAnalysys("test.dc");
  string tok_str[] = {
    "TOK_INT",
    "TOK_EOF",
    "TOK_IDENTIFIER",
    "TOK_DIGIT",
    "TOK_SYMBOL",
    "TOK_RETURN",
    "TOK_UNKNOWN",
  };
  for(int i = 0; i < TokenStream::Instance()->getTokenSize(); i++) {
    Token* tok = TokenStream::Instance()->getToken(i);
    cout << tok->getString() << ": " << tok_str[tok->getType()] << endl;
  }
}

