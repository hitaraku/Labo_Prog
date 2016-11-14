#ifndef __AST_H
#define __AST_H

enum AST_ID {
  BaseID,
  VariableID,
  NumberID,
  BinaryExprID,
  VariableDeclID,
  JumpStmtID
};

class BaseAST
{
public:
  BaseAST(AST_ID id) : m_ID(id) {}
  virtual ~BaseAST() = 0;
  AST_ID getValueID() const { return m_ID; }

private:
  AST_ID m_ID;
};

class VariableAST : public BaseAST
{
public:
  VariableAST(const std::string &name) :
    BaseAST(VariableID),
    m_name(name)
  { }
  virtual ~VariableAST() { }

  static inline bool classof(VariableAST const *) { return true; }
  static inline bool classof(BaseAST const *base) { return base->getValueID() == VariableID; }

  std::string getName() { return m_name; }

private:
  std::string m_name;
};

class NumberAST : public BaseAST
{
public:
  NumberAST(int num) :
  BaseAST(NumberID),
  m_num(num)
  {}
  virtual ~NumberAST() {}

  static inline bool classof(VariableAST const *) { return true; }
  static inline bool classof(BaseAST const *base) { return base->getValueID() == VariableID; }

  int getNum() { return m_num; }

private:
  int m_num;
};

class JumpStmtAST : public BaseAST
{
public:
  JumpStmtAST(BaseAST* expr) :
    BaseAST(JumpStmtID),
    m_expr(expr)
  {}
  ~JumpStmtAST() { SAFE_DELETE(m_expr); }

  static inline bool classof(JumpStmtAST const *) { return true; }
  static inline bool classof(BaseAST const *base) { return base->getValueID() == JumpStmtID; }

  BaseAST* getExpr() { return m_expr; }

private:
  BaseAST* m_expr;
};

class BinaryExprAST : public BaseAST
{
public:
  BinaryExprAST(std::string opr, BaseAST *rhs, BaseAST *lhs) :
    BaseAST(BinaryExprID),
    m_opr(opr),
    m_rhs(rhs),
    m_lhs(lhs)
  {}
  virtual ~BinaryExprAST() {
    SAFE_DELETE(m_rhs);
    SAFE_DELETE(m_lhs);
  }

  static inline bool classof(BinaryExprAST const *) { return true; }
  static inline bool classof(BaseAST const *base) { return base->getValueID() == BinaryExprID; }

  std::string getOpr() { return m_opr; }
  BaseAST *getLHS() { return m_lHS; }
  BaseAST *getRHS() { return m_rhs; }

private:
  std::string m_opr;
  BaseAST *m_rhs;
  BaseAST *m_lhs;
};

class CallExprAST : public BaseAST
{
public:
  CallExprAST(const std::string &callee, std::vector<BaseAST*> &args) :
    BaseAST(CallExprID),
    m_callee(callee),
    m_args(args)
  {}
  virtual ~CallExprAST() {}

  static inline bool classof(VariableDeclAST const *) { return true; }
  static inline bool classof(BaseAST const *base) { return base->getValueID() == VariableDeclID; }

  std::string getCallee() { return m_callee; }
  BaseAST* getArgs(int i) {
    if(i < m_args.size()) {
      return m_args[i];
    } else {
      return NULL;
    }
  }

private:
  std::string m_callee; // function name
  std::vector<BaseAST*> m_args; 
};

class VariableDeclAST : public BaseAST
{
public:
  enum DeclType {
    param,
    local
  };

  VariableDeclAST(const std::string &name) :
    BaseAST(VariableDeclID),
    m_name(name)
  { }
  virtual ~VariableDeclAST() { }

  static inline bool classof(VariableDeclAST const *) { return true; }
  static inline bool classof(BaseAST const *base) { return base->getValueID() == VariableDeclID; }

  // getter
  std::string getName() { return m_name; }
  DeclType getType() { return m_type; }

  // setter
  bool setDeclType(DeclType type) {
    m_type = type;
    return true;
  }

private:
  std::string m_name;
  DeclType m_type;
};

class FunctionStmtAST : public BaseAST
{
public:
  FunctionStmtAST() : {}
  virtual ~FunctionStmtAST() {}

  bool addVariableDeclaration(VariableDeclAST *vdecl);
  bool addStatement(BaseAST *stmt) { StmtLists.push_back(stmt); }

  VariableDeclAST *getVariableDecl(int i) {
    if(i < m_variableDecls.size()) {
      return m_variableDecls.at(i);
    } else {
      return NULL;
    }
  }

  BaseAST* getStatement(int i) {
    if(i < m_stmtLists.size()) {
      return m_stmtLists.at(i);
    } else {
      return NULL;
    }
  }

private:
  std::vector<VariableDeclAST*> m_variableDecls;
  std::vector<BaseAST*> m_stmtLists;
};

class PrototypeAST
{
public:
  PrototypeAST(const std::string &name, const std::vector<std::string> &params) :
    m_name(name),
    m_params(params)
  {}
  virtual ~PrototypeAST() {}

  static inline bool classof(VariableDeclAST const *) { return true; }
  static inline bool classof(BaseAST const *base) { return base->getValueID() == VariableDeclID; }

  std::string getName() { return m_name; }
  std::string getParam(int i) {
    if(i < m_params.size()) {
      return m_params[i];
    } else {
      return NULL;
    }
  }
  int getParamNum() { return m_params.size(); }

private:
  std::string m_name;
  std::vector<std::string> m_params;
};

class FunctionAST
{
public:
  FunctionAST(PrototypeAST *proto, FunctionStmtAST *body) :
    m_proto(proto),
    m_body(body)
  {}
  virtual ~FunctionAST() {}

  std::string getName() { return m_proto->getName(); }
  PrototypeAST *getPrototype() { return m_proto; }
  FunctionStmtAST *getBody() { return m_body; }

private:
  PrototypeAST *m_proto;
  FunctionStmtAST *m_body;
};

class TranslationUnitAST
{
public:
  TranslationUnitAST() { }
  virtual ~TranslationUnitAST() { }

  bool addPrototype(PrototypeAST *proto);
  bool addFunction(FunctionAST *func);

  PrototypeAST *getPrototype(int i) {
    if(i < m_prototypes.size) {
      return m_prototypes.at(i);
    } else {
      return NULL;
    }
  }

  FunctionAST *getFunction(int i) {
    if(i < m_functions.size()) {
      return m_functions.at(i);
    } else {
      return NULL;
    }
  }

private:
  std::vector<PrototypeAST*> m_prototypes;
  std::vector<FunctionAST*> m_functions;
};

#endif // __AST_H
