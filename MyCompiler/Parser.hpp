#ifndef __PARSER_H_
#define __PARSER_H_

#include<algorithm>
#include<cstdio>
#include<cstdlib>
#include<map>
#include<string>
#include<vector>
#include <fstream>
#include"APP.hpp"
#include"AST.hpp"
#include "Token.hpp"
#include "TokenStream.hpp"

class Parser
{
public:
  Parser(std::string filename);
  ~Parser() {
    SAFE_DELETE(m_TU);
  }
  bool doParse();
  TranslationUnitAST &getAST();

private:
  bool visitTranslationUnit();
  bool visitExternalDeclaration();
  PrototypeAST *visitFunctionDeclaration();
  FunctionAST *visitFunctionDefinition();
  PrototypeAST *visitPrototype();
  FunctionStmtAST *visitFunctionStatement(PrototypeAST *proto);
  VariableDeclAST *visitVariableDeclaration();
  BaseAST *visitStatement();
  BaseAST *visitExpressionStatement();
  BaseAST *visitJumpStatement();
  BaseAST *visitAssignmentExpression();
  BaseAST *visitAdditiveExpression(BaseAST *lhs);
  BaseAST *visitMultiplicativeExpression(BaseAST *lhs);
  BaseAST *visitPostfixExpression();
  BaseAST *visitPrimaryExpression();

private:
  void LexicalAnalysys(std::string filename);

private:
  TokenStream *m_tokens;
  TranslationUnitAST *m_TU;

  std::vector<std::string> m_variableTable;
  std::map<std::string, int> m_prototypeTable;
  std::map<std::string, int> m_functionTable;
};

#endif /* __PARSER_H_ */
