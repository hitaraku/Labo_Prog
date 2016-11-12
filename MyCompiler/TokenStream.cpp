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

