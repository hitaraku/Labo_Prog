#ifndef AST_HPP
#define AST_HPP

#include <vector>
#include <map>
#include <string>
#include <llvm/Support/Casting.h>

#include "APP.hpp"

// class Declaration is refered by after class.
class BaseAST;
class TranslationUnitAST;
class PrototypeAST;
class FunctionAST;
class FunctionStatementAST;
class VariableDeclarationAST;
class BinaryExprAST;
class NullExprAST;
class CallExprAST;
class JumpStatementAST;
class VariableAST;
class NumberAST;

enum AstID {
  BaseID,
  TranslationUnitID,
  PrototypeID,
  FunctionID,
  FunctionStatementID,
  VariableDeclarationID,
  BinaryExprID,
  NullExprID,
  CallExprID,
  JumpStatementID,
  VariableID,
  NumberID
};

// This Class is AST of base class

class BaseAST {
private:
  AstID m_ID;

public:
  BaseAST(AstID id) : m_ID(id) {}
  virtual ~BaseAST() {}
  AstID getValueID() const { return m_ID; }
};

class TranslationUnitAST {
private:
  std::vector<PrototypeAST*> m_Prototypes;
  std::vector<FunctionAST*> m_Functions;

public:
  TranslationUnitAST() {}
  ~TranslationUnitAST();
  bool addPrototype(PrototypeAST *proto);
  bool addFunction(FunctionAST *func);
  bool empty();
  PrototypeAST *getPrototype(int i) {
    if(i < m_Prototypes.size()) {
      return m_Prototypes.at(i);
    } else {
      return NULL;
    }
  }
  FunctionAST *getFunction(int i) {
    if(i < m_Functions.size()) {
      return m_Functions.at(i);
    } else {
      return NULL;
    }
  }
};

// This AST express function declaration.
class PrototypeAST
{
private:
  std::string m_Name;
  std::vector<std::string> m_Params;

public:
  PrototypeAST(const std::string &name, const std::vector<std::string> &params) :
    m_Name(name),
    m_Params(params) {}
  std::string getName() { return m_Name; }
  std::string getParamName(int i) {
    if(i < m_Params.size()) { return m_Params.at(i); }
    return NULL;
  }
  int getParamNum() { return m_Params.size(); }
};

// This AST express function definition.
class FunctionAST
{
private:
  PrototypeAST *m_Proto;
  FunctionStatementAST *m_Body;

public:
  FunctionAST(PrototypeAST *proto, FunctionStatementAST *body) :
    m_Proto(proto),
    m_Body(body)
  {}
  ~FunctionAST();
  std::string getName() { return m_Proto->getName(); }
  PrototypeAST *getPrototype() { return m_Proto; }
  FunctionStatementAST *getBody() { return m_Body; }
};

// This AST express function definition containts.
class FunctionStatementAST {
private:
  std::vector<VariableDeclarationAST*> m_VariableDecls;
  std::vector<BaseAST*> m_StatementLists;

public:
  FunctionStatementAST() {}
  ~FunctionStatementAST();
  bool addVariableDeclaration(VariableDeclarationAST *vdecl);
  void addStatement(BaseAST *statement) { m_StatementLists.push_back(statement); }
  VariableDeclarationAST *getVariableDecl(int i) {
    if(i < m_VariableDecls.size()) {
      return m_VariableDecls.at(i);
    } else {
      return NULL;
    }
  }
  BaseAST *getStatement(int i) {
    if(i < m_StatementLists.size()) {
      return m_StatementLists.at(i);
    } else {
      return NULL;
    }
  }
};

class VariableDeclarationAST : public BaseAST {
public:
  typedef enum {
    param,
    local
  } DeclType;

private:
  std::string m_Name;
  DeclType m_Type;

public:
  VariableDeclarationAST(const std::string &name) :
    BaseAST(VariableDeclarationID),
    m_Name(name)
  {}
  static inline bool classof(VariableDeclarationAST const*) { return true; }
  static inline bool classof(BaseAST const* base) {
    return base->getValueID() == VariableDeclarationID;
  }
  ~VariableDeclarationAST() {}
  bool setDeclType(DeclType type) {
    m_Type = type;
    return true;
  }
  std::string getName() { return m_Name; }
  DeclType getType() { return m_Type; }
};

// This AST express binary expression
class BinaryExprAST : public BaseAST
{
private:
  std::string m_Op;
  BaseAST *m_LHS, *m_RHS;

public:
  BinaryExprAST(std::string op, BaseAST *lhs, BaseAST *rhs) :
    BaseAST(BinaryExprID),
    m_Op(op),
    m_LHS(lhs),
    m_RHS(rhs)
  {}
  ~BinaryExprAST() {
    SAFE_DELETE(m_LHS);
    SAFE_DELETE(m_RHS);
  }
  static inline bool classof(BinaryExprAST const*) { return true; }
  static inline bool classof(BaseAST const* base) {
    return base->getValueID() == BinaryExprID;
  }
  std::string getOp() { return m_Op; }
  BaseAST *getLHS() { return m_LHS; }
  BaseAST *getRHS() { return m_RHS; }
};

// This AST express ";"

class NullExprAST : public BaseAST {
public:
  NullExprAST() : BaseAST(NullExprID) {}
  static inline bool classof(NullExprAST const*) { return true; }
  static inline bool classof(BaseAST const* base) {
    return base->getValueID() == NullExprID;
  }
};

// This AST express call function.
class CallExprAST : public BaseAST {
private:
  std::string m_Callee;
  std::vector<BaseAST*> m_Args;

public:
  CallExprAST(const std::string &callee, std::vector<BaseAST*> &args) :
    BaseAST(CallExprID),
    m_Callee(callee),
    m_Args(args)
  {}
  ~CallExprAST();
  std::string getCallee() { return m_Callee; }
  BaseAST *getArgs(int i) {
    if(i < m_Args.size()) {
      m_Args.at(i);
    } else {
      return NULL;
    }
  }
  static inline bool classof(CallExprAST const*) { return true; }
  static inline bool classof(BaseAST const* base) {
    return base->getValueID() == CallExprID;
  }
};

class JumpStatementAST : public BaseAST {
private:
  BaseAST *m_Expr;

public:
  JumpStatementAST(BaseAST *expr) :
  BaseAST(JumpStatementID),
  m_Expr(expr)
  {}
  ~JumpStatementAST() { SAFE_DELETE(m_Expr); }
  BaseAST *getExpr() { return m_Expr; }
  static inline bool classof(JumpStatementAST const*) { return true; }
  static inline bool classof(BaseAST const* base) {
    return base->getValueID() == JumpStatementID;
  }
};

class VariableAST : public BaseAST {
private:
  std::string m_Name;

public:
  VariableAST(const std::string &name) :
    BaseAST(VariableID),
    m_Name(name)
  {}
  ~VariableAST() {}
  static inline bool classof(VariableAST const*) { return true; }
  static inline bool classof(BaseAST const* base) {
    return base->getValueID() == VariableID;
  }
  std::string getName() { return m_Name; }
};

class NumberAST : public BaseAST {
private:
  int m_Val;

public:
  NumberAST(int val) :
  BaseAST(NumberID),
  m_Val(val)
  {}
  ~NumberAST() {}
  int getNumberValue() { return m_Val; }
  static inline bool classof(NumberAST const*) { return true; }
  static inline bool classof(BaseAST const* base) {
    return base->getValueID() == NumberID;
  }
};

#endif /* AST_HPP */

