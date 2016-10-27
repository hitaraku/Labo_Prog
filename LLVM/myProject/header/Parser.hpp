#ifndef PARSER_HPP
#define PARSER_HPP

#include "TokenStream.hpp"

class Parser {
private:
  TokenStream *mp_Tokens;
  TranslationUnitAST *mp_TU;

  // table of identifie of analysys
  std::vector<std::string> mv_VariableTable;
  std::map<std::string, int> mm_PrototypeTable;
  std::map<std::string, int> mm_FunctionTable;

  // parse target filename
  std::string m_filename;

public:
  ~Parser(){ SAFE_DELETE(mp_TU); SAFE_DELETE(mp_Tokens); }
  Parser(std::string filename);
  bool doParse();
  TranslationUnitAST &getAST();

private:

  // LexicalAnalysys
  TokenStream LexicalAnalysys();
  
  /////////////////////////////
  // literal analysys method //
  /////////////////////////////
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
  BaseAST *visitAssignmentExpression();
  BaseAST *visitAdditiveExpression(BaseAST *lhs);
  BaseAST *visitMultiplicativeExpression(BaseAST *lhs);
  BaseAST *visitPostfixExpression();
  BaseAST *visitPrimaryExpression();
};

#endif /* PARSER_HPP */
