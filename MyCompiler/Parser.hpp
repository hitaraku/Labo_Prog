#ifndef __PARSER_H_
#define __PARSER_H_

#include "TokenStream.hpp"

class Parser
{
public:
  Parser(std::string filename);
  ~Parser() {
    SAFE_DELETE(m_TU);
    SAFE_DELETE(m_tokens);
  }
  bool doParse();
  TranslationUnitAST &getAST();

private:
  bool visitTranslationUnit();
  bool visitExternalDeclaration(TranslationUnitAST *tunit);
  PrototypeAST *visitFunctionDeclaration();
  FunctionAST *visitFunctionDefinition();
  PrototypeAST *visitPrototype();
  FunctionStmtAST *visitFunctionStatement(PrototypeAST *proto);
  VariableDeclAST *visitVariableDeclaration();
  BaseAST *visitStatement();
  BaseAST *visitExpressionStatement();
  BaseAST *visitJumpStatement();
  BaseAST *visitAssignmentExpression(BaseAST *lhs);
  BaseAST *visitMultiplicativeExpression(BaseAST *lhs);
  BaseAST *visitPostfixExpression();
  BaseAST *visitPrimaryExpression();

private:
  TokenStream* LexicalAnalysys(std::string filename);

private:
  TokenStream *m_tokens;
  TranslationUnitAST *m_TU;
};

#endif /* __PARSER_H_ */
