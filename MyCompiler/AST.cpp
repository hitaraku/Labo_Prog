#include "AST.hpp"

TranslationUnitAST::~TranslationUnitAST()
{
  for(int i = 0; i < m_prototypes.size(); ++i)
  {
    SAFE_DELETE(m_prototypes[i]);
  }
  m_prototypes.clear();

  for(int i = 0; i < m_functions.size(); ++i)
  {
    SAFE_DELETE(m_functions[i]);
  }
  m_functions.clear();
}

bool TranslationUnitAST::addPrototype(PrototypeAST* proto)
{
  m_prototypes.push_back(proto);
  return true;
}

bool TranslationUnitAST::addFunction(FunctionAST* func)
{
  m_functions.push_back(func);
  return true;
}

bool TranslationUnitAST::empty()
{
  if(m_prototypes.size() == 0 && m_functions.size() == 0) {
    return true;
  } else {
    return false;
  }
}

FunctionAST::~FunctionAST()
{
  SAFE_DELETE(m_proto);
  SAFE_DELETE(m_body);
}

FunctionStmtAST::~FunctionStmtAST()
{
  for(int i = 0; i < m_variableDecls.size(); i++) {
    SAFE_DELETE(m_variableDecls[i]);
  }
  m_variableDecls.clear();

  for(int i = 0; i < m_stmtLists.size(); i++) {
    SAFE_DELETE(m_stmtLists[i]);
  }
  m_stmtLists.clear();
}

bool FunctionStmtAST::addVariableDeclaration(VariableDeclAST *vdecl){
		m_variableDecls.push_back(vdecl);
		return true;
}

CallExprAST::~CallExprAST()
{
  for(int i = 0; i < m_args.size(); ++i) {
    SAFE_DELETE(m_args[i]);
  }
  m_args.clear();
}
