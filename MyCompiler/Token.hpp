#ifndef __TOKEN_HPP
#define __TOKEN_HPP

#include <string>

enum TOKEN_TYPE{
  TOK_INT,
  TOK_EOF,
  TOK_IDENTIFIER,
  TOK_DIGIT,
  TOK_SYMBOL,
  TOK_RETURN,
  TOK_UNKNOWN
};

class Token
{
private:
  std::string m_token;
  TOKEN_TYPE m_type;
  int m_num; // if m_type num

public:
  Token(std::string token, TOKEN_TYPE type) :
    m_token(token),
    m_type(type),
    m_num(0)
  {
    if(type == TOK_DIGIT) {
      m_num = std::stoi(m_token);
    }
  }
  ~Token() {}

  std::string getString() { return m_token; }
  TOKEN_TYPE getType() { return m_type; }
  int getNum() { return m_num; }
};

#endif // __TOKEN_HPP
