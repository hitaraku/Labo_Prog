#include <iostream>
#include "TokenStream.hpp"

TokenStream *TokenStream::m_tokenStream = 0;

bool TokenStream::checkTokenIndex(int i) {
  if(0 <= i && i < m_tokens.size()) {
    return true;
  } else {
    return false;
  }
}

Token* TokenStream::getToken(int i) {
  if(checkTokenIndex(i)) {
    return m_tokens[i];
  } else {
    std::cout << "This index " << i << " is illigal." << std::endl;
    return NULL;
  }
}

void TokenStream::setToken(Token* token){ m_tokens.push_back(token); ++m_index; }
std::string TokenStream::getCurString() { return m_tokens[m_index-1]->getString(); }
TOKEN_TYPE TokenStream::getCurType() { return m_tokens[m_index-1]->getType(); }
int TokenStream::getCurNum() { return m_tokens[m_index-1]->getNum(); }
void TokenStream::getNextToken() { ++m_index; }
void TokenStream::unsetToken(int i) { m_index -= i; }
int TokenStream::getCurIndex() { return m_index; }
void TokenStream::applyTokenIndex(int i) { m_index = i; }
