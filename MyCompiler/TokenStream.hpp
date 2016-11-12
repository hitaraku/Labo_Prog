#ifndef __TOKENSTREAM_HPP
#define __TOKENSTREAM_HPP

#include <vector>

#include "Token.hpp"

class TokenStream
{
private:
  TokenStream() : m_index(0) {}
  ~TokenStream() {
    m_tokens.clear();
    delete m_tokenStream;
  }

  bool checkTokenIndex(int i);

public:
  static TokenStream* Instance() {
    if(m_tokenStream == NULL) {
      m_tokenStream = new TokenStream;
    } else {
      return m_tokenStream;
    }
  }

  Token* getToken(int i);
  int getTokenSize() { return m_tokens.size(); }
  void setToken(Token* token) { m_tokens.push_back(token); ++m_index; }

private:
  std::vector<Token*> m_tokens;
  static TokenStream *m_tokenStream;
  int m_index;
};

#endif // __TOKENSTREAM_HPP
