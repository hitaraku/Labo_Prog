#include <iostream>
#include <fstream>
using namespace std;

#include "TokenStream.hpp"

void LaxycalAnalysys(std::string filename)
{
  fstream fs("test.dc", ios::in);
  string str;
  Token *token;

  for(;;) {
    if(fs.eof()) {
      break;
    } else {
      string ident;
      bool iden_bool = false;
      getline(fs, str);
      for(int index = 0; index < str.size(); ++index) {
        if(str.at(index) == ' ') {
          if(iden_bool) {
            token = new Token(ident, TOK_IDENTIFIER);
            TokenStream::Instance()->setToken(token);
            ident = "";
            iden_bool = false;
          }
          continue;
        }
        // return value int
        else if(str.at(index) == 'i' && str.at(index + 1) == 'n' && str.at(index + 2) == 't') {
          if(iden_bool) {
            token = new Token(ident, TOK_IDENTIFIER);
            TokenStream::Instance()->setToken(token);
            ident = "";
            iden_bool = false;
          }
          token = new Token("int", TOK_INT);
          index += 2;
          TokenStream::Instance()->setToken(token);
        }
        // comment
        else if(str.at(index) == '/' && str.at(index + 1) == '/') { 
          if(iden_bool) {
            token = new Token(ident, TOK_IDENTIFIER);
            TokenStream::Instance()->setToken(token);
            ident = "";
            iden_bool = false;
          }
          index = str.size();
        }
        // return
        else if(str.at(index) == 'r' && str.at(index + 1) == 'e'
           && str.at(index + 2) == 't' && str.at(index + 3) == 'u'
           && str.at(index + 4) == 'r' && str.at(index + 5) == 'n')
        {
          if(iden_bool) {
            token = new Token(ident, TOK_IDENTIFIER);
            TokenStream::Instance()->setToken(token);
            ident = "";
            iden_bool = false;
          }
          token = new Token("return", TOK_RETURN);
          index += 5;
          TokenStream::Instance()->setToken(token);
        }
        // digit
        else if(isdigit(str.at(index))) {
          if(iden_bool) {
            token = new Token(ident, TOK_IDENTIFIER);
            TokenStream::Instance()->setToken(token);
            ident = "";
            iden_bool = false;
          }
          int i = 1;
          for(;;i++) {
            if(isdigit(str.at(index + i))) {
              // no 
            } else {
              break;
            }
          }
          token = new Token(str.substr(index, i), TOK_INT);
          TokenStream::Instance()->setToken(token);
        }
        else if(str.at(index) == '*' || str.at(index) == '+' || str.at(index) == '-' ||
           str.at(index) == '/' || str.at(index) == ';' || str.at(index) == '{' ||
           str.at(index) == '}' || str.at(index) == '(' || str.at(index) == ')' ||
           str.at(index) == '=' )
        {
          if(iden_bool) {
            token = new Token(ident, TOK_IDENTIFIER);
            TokenStream::Instance()->setToken(token);
            ident = "";
            iden_bool = false;
          }
          token = new Token(str.substr(index, 1), TOK_SYMBOL);
          TokenStream::Instance()->setToken(token);
        }
        else
        {
          ident += str.substr(index, 1);
          iden_bool = true;
        }
      }
    }
  }

  fs.close();
}

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

