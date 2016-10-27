#include "Parser.hpp"
#include "Token.hpp"
#include "APP.hpp"
#include <fstream>

Parser::Parser(std::string filename) : m_filename(filename) {
  mp_Tokens = LexicalAnalysis;   
}

TokenStream Parser::*LexicalAnalysis() {
  TokenStream *tokens = new TokenStream();
  std::fstream ifs;
  std::string cur_line;
  std::string token_str;
  int line_num = 0;
  bool iscomment = false;

  ifs.open(m_filename.c_str(), std::ios::in);
  if(!ifs) { return NULL; }
  while(ifs && getline(ifs, cur_line)) {
    char next_char;
    std::string line;
    Token *next_token;
    int index = 0;
    int length = cur_line.length();

    while(index < length) {
      next_char = cur_line.at(index++);

      // jump commentout
      if(iscomment) {
        // if last 2 characters are not comment closure, will jump load 2 characters .
        if((length - index) < 2 || (cur_line.at(index) != '*') || (cur_line.at(index++) != '/')) {
          continue;
        } else {
          isconnemt = false;
        }
      }

      if(next_char == EOF) { /* EOF */
        token_str = EOF;
        next_token = new Token(token_str, TOK_EOF, line_num);
      } else if(isspace(next_char)) { /* SPACE */
        continue;
      } else if(isalpha(next_char)) { /* IDENTIFIER */
        token_str += next_char;
        next_char = cur_line.at(index++);
        while(isalnum(next_char)) {
          token_str += next_char;
          next_char = cur_line.at(index++);
          if(index == length) { break; }
        }
        index--;

        if(token_str == "int") {
          next_token = new Token(token_str, TOK_INT, line_num);
        } else if(token_str == "return") {
          next_token = new Token(token_str, TOK_RETURN, line_num);
        } else {
          next_token = new Token(token_str, TOK_IDENTIFIER, line_num);
        }
      } else if(isdigit(next_char)) {
        if(next_char == '0') {
          token_str += next_char;
          next_token = new Token(token_str, TOK_DIGIT, line_num);
        } else {
          token_str += next_char;
          next_char = cur_line.at(index++);
          while(isdigit(next_char)) {
            token_str += next_char;
            next_char = cur_line.at(index++);
          }
          next_token = new Token(token_str, TOK_DIGIT, line_num);
          index--;
        }
      } else if(next_char == '/') {
        token_str += next_char;
        next_char = cur_line.at(index++);

        // if comment
        if(next_char == '/') {
          iscomment = true;
          continue;
        } else if(next_char == '*') {
          iscomment = true;
          continue;
        } else { /* DIVIDER('/') */
          index--;
          next_token = new Token(token_str, TOK_SYMBOL, line_num);
        }
      } else { /* OTHER TOKEN */
        if(next_char == '*' || next_char == '+' || next_char == '-' || next_char == '.' || next_char == ',' ||
           next_char == '(' || next_char == ')' || next_char == '{' || next_char == '}' || next_char == ';' ||
           next_char == '=')
        {
          token_str += next_char;
          next_token = new Token(token_str, TOK_SYMBOL, line_num);
        } else { /* This token impossible parse. */
          fprintf(stderr, "unclear token : %c", next_char);
          SAFE_DELETE(tokens);
          return NULL;
        }
      }

      // add Tokens
      tokens->pushToken(next_token);
      token_str.clear();
    }
    token_str.clear();
    line_num++;
  }

  if(ifs.eof()) {
    tokens->pushToken(new Token(token_str, TOK_EOF, line_num));
  }
  ifs.close();
  tokens->printTokens();
  return tokens;
}
