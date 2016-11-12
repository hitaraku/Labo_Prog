#include "AST.hpp"

TranslationUnitAST::~TranslationUnitAST() {
  for(int i = 0; i < m_Prototypes.size(); i++) {
    SAFE_DELETE(m_Prototypes[i]);
  }
  m_Prototypes.clear();

  for(int i = 0; i < m_Functions.size(); i++) {
    SAFE_DELETE(m_Functions[i]);
  }
  m_Functions.clear();
}

bool TranslationUnitAST::addPrototype(PrototypeAST *proto) {
  m_Prototypes.push_back(proto);
  return true;
}

bool TranslationUnitAST::addFunction(FunctionAST *func) {
  m_Functions.push_back(func);
  return true;
}

bool TranslationUnitAST::empty() {
  if(m_Prototypes.size() == 0 && m_Functions.size() == 0) {
    return true;
  } else {
    return false;
  }
}

FunctionAST::~FunctionAST() {
  SAFE_DELETE(m_Proto);
  SAFE_DELETE(m_Body);
}

FunctionStatementAST::~FunctionStatementAST() {
  for(int i = 0; i < m_VariableDecls.size(); i++) {
    SAFE_DELETE(m_VariableDecls[i]);
  }
  m_VariableDecls.clear();

  for(int i = 0; i < m_StatementLists.size(); i++) {
    SAFE_DELETE(m_StatementLists[i]);
  }
  m_StatementLists.clear();
}

bool FunctionStatementAST::addVariableDeclaration(VariableDeclarationAST *vdecl) {
  m_VariableDecls.push_back(vdecl);
  return true;
}

CallExprAST::~CallExprAST() {
  for(int i = 0; i < m_Args.size(); i++) {
    SAFE_DELETE(m_Args[i]);
  }
}
