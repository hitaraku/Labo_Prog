#ifndef TOKENSTREAM_HPP
#define TOKENSTREAM_HPP

#include <vector>
#include "Token.hpp"

class TokenStream {
private:
  std::vector<Token*> c_tokens;
  int c_curIndex;

public:

  TokenStream() : c_curIndex(0) {}
  ~TokenStream();

  bool ungetToken(int Times=1);
  bool getNextToken();
  bool pushToken(Token *token) {
    c_tokens.push_back(token);
    return true;
  }
  Token getToken();
  TokenType getCurType() { return c_tokens[c_curIndex]->getTokenType(); }
  std::string getCurString() { return c_tokens[c_curIndex]->getTokenString(); }
  int getCurNumVal() { return c_tokens[c_curIndex]->getNumberValue(); }
  bool printTokens();
  int getCurIndex() { return c_curIndex; }
  bool applyTokenIndex(int index) { c_curIndex=index; return true; }
};

#endif /* TOKENSTREAM_HPP */
