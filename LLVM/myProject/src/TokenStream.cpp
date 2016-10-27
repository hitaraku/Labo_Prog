#include "TokenStream.hpp"
#include "APP.hpp"
#include <vector>
#include <cstdlib>
#include <cstdio>

TokenStream::~TokenStream() {
  for(int i = 0; i < c_tokens.size(); i++) {
    SAFE_DELETE(c_tokens[i]);
  }
  c_tokens.clear();
}

Token TokenStream::getToken() {
  return *c_tokens[c_curIndex];
}

bool TokenStream::getNextToken() {
  int size = c_tokens.size();
  if(--size == c_curIndex) {
    return false;
  } else if(c_curIndex < size) {
    c_curIndex++;
    return true;
  } else {
    return false;
  }
}

bool TokenStream::ungetToken(int times) {
  for(int i = 0; i < times; i++) {
    if(c_curIndex == 0) {
      return false;
    } else {
      c_curIndex--;
    }
  }
  return true;
}

bool TokenStream::printTokens() {
  char tokenTypeName[6][15] = {"TOK_IDENTIFIER", "TOK_DIGIT", "TOK_SYMBOL", "TOK_INT", "TOK_RETURN", "TOK_EOF"};
  std::vector<Token*>::iterator titer = c_tokens.begin();
  while(titer != c_tokens.end()) {
    fprintf(stdout, "%s:", tokenTypeName[(*titer)->getTokenType()]);
    if((*titer)->getTokenType() != TOK_EOF) {
      fprintf(stdout, "%s\n", (*titer)->getTokenString().c_str());
    }
    ++titer;
  }
  return true;
}
