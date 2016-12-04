#ifndef __TOKENSTREAM_HPP
#define __TOKENSTREAM_HPP

#include <vector>
#include <iostream>

#include "Token.hpp"

class TokenStream
{
public:
  TokenStream() : m_index(0) {}
  ~TokenStream() {
    std::cout << "called tokenstream destructor" << std::endl;
    m_tokens.clear();
  }

private:  
  bool checkTokenIndex(int i);

public:
  Token* getToken(int i);
  int getTokenSize() { return m_tokens.size(); }
  void setToken(Token* token);

  // return current status
  std::string getCurString() ;
  TOKEN_TYPE getCurType() ;
  int getCurNum() ;
  void getNextToken() ;
  void unsetToken(int i) ;
  int getCurIndex() ;
  void applyTokenIndex(int i) ;

private:
  std::vector<Token*> m_tokens;
  int m_index;
};

#endif // __TOKENSTREAM_HPP
