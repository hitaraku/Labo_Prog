#ifndef PARSER_HPP
#define PARSER_HPP

#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>

#include "TokenStream.hpp"
#include "AST.hpp"
#include "APP.hpp"

class Parser {
private:
  TokenStream *m_Tokens;
  TranslationUnitAST *m_TU;

  // table of identifie of analysys
  std::vector<std::string> m_VariableTable;
  std::map<std::string, int> m_PrototypeTable;
  std::map<std::string, int> m_FunctionTable;

public:
  Parser(std::string filename);
  ~Parser(){ SAFE_DELETE(m_TU); SAFE_DELETE(m_Tokens); }
  bool doParse();
  TranslationUnitAST &getAST();

private:

  // LexicalAnalysis
  TokenStream *LexicalAnalysis(std::string filename);
  
  /////////////////////////////
  // literal analysys method //
  /////////////////////////////
  bool visitTranslationUnit();
  bool visitExternalDeclaration(TranslationUnitAST *tunit);
  PrototypeAST *visitFunctionDeclaration();
  FunctionAST *visitFunctionDefinition();
  PrototypeAST *visitPrototype();
  FunctionStatementAST *visitFunctionStatement(PrototypeAST *proto);
  VariableDeclarationAST *visitVariableDeclaration();
  BaseAST *visitStatement();
  BaseAST *visitExpressionStatement();
  BaseAST *visitJumpStatement();
  BaseAST *visitAssignmentExpression();
  BaseAST *visitAdditiveExpression(BaseAST *lhs);
  BaseAST *visitMultiplicativeExpression(BaseAST *lhs);
  BaseAST *visitPostfixExpression();
  BaseAST *visitPrimaryExpression();
};

#endif /* PARSER_HPP */
