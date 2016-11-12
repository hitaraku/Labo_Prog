#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <cstdlib>

enum TokenType {
  TOK_IDENTIFIER, 
  TOK_DIGIT, 
  TOK_SYMBOL, 
  TOK_INT, 
  TOK_RETURN, 
  TOK_EOF
};

class Token {

private:
  TokenType c_type;
  std::string c_tokenString;
  int c_number;
  int c_line;

public:
  Token(std::string tokenString, TokenType type, int line) :
    c_tokenString(tokenString),
    c_type(type),
    c_line(line)
  {
    if(c_type == TOK_DIGIT) {
      c_number = atoi(tokenString.c_str());
    } else {
      c_number = 0x7fffffff;
    }
  }
  ~Token() {}

  TokenType getTokenType() { return c_type; }
  std::string getTokenString() { return c_tokenString; }
  int getNumberValue() { return c_number; }
};

#endif /* TOKEN_HPP */
