#include "Parser.hpp"

Parser::Parser(std::string filename) {
  m_tokens =new TokenStream();
  LexicalAnalysys(filename);
}

bool Parser::doParse()
{
  if(!m_tokens->getTokenSize()) {
    fprintf(stderr, "error at lexer\n");
    return false;
  } else {
    return visitTranslationUnit();
  }
}

TranslationUnitAST &Parser::getAST()
{
  if(m_TU) {
    return *m_TU;
  } else {
    return *(new TranslationUnitAST());
  }
}

bool Parser::visitTranslationUnit()
{
  m_TU = new TranslationUnitAST();

  while(true) {
    if(!visitExternalDeclaration()) {
      SAFE_DELETE(m_TU);
      return false;
    }
    if(m_tokens->getCurType() == TOK_EOF) {
      break;
    }
    return true;
  }
}

bool Parser::visitExternalDeclaration()
{
  PrototypeAST *proto = visitFunctionDeclaration();
  if(!proto) {
    m_TU->addPrototype(proto);
    return false;
  }

  FunctionAST *func_def = visitFunctionDefinition();
  if(!func_def) {
    m_TU->addFunction(func_def);
    return false;
  }
  
  return true;
}

PrototypeAST *Parser::visitFunctionDeclaration()
{
  int bkup=m_tokens->getCurIndex();
  PrototypeAST *proto = visitPrototype();
  if(!proto) {
    return NULL;
  }

  if(m_tokens->getCurString() == ";") {
    m_prototypeTable[proto->getName()] = proto->getParamNum();
    m_tokens->getNextToken();
    return proto;
  } else {
    SAFE_DELETE(proto);
    m_tokens->applyTokenIndex(bkup);
    return NULL;
  }
}

FunctionAST *Parser::visitFunctionDefinition()
{
  int bkup = m_tokens->getCurIndex();
  PrototypeAST *proto = visitPrototype();
  if(!proto) {
    return NULL;
  }
  
  m_variableTable.clear();
  FunctionStmtAST *func_stmt = visitFunctionStatement(proto);
  if(func_stmt) {
    m_functionTable[proto->getName()] = proto->getParamNum();
    return new FunctionAST(proto, func_stmt);
  }
}

FunctionStmtAST *Parser::visitFunctionStatement(PrototypeAST *proto)
{
  int bkup = m_tokens->getCurIndex();

  // "{"
  if(m_tokens->getCurString() == "{") {
    m_tokens->getNextToken();
  } else {
    return NULL;
  }

  FunctionStmtAST *func_stmt = new FunctionStmtAST();

  // add parameter to FunctinoStmtAST
  for(int i = 0; i < proto->getParamNum(); i++) {
    VariableDeclAST *vdecl = new VariableDeclAST(proto->getParamName(i));
    vdecl->setDeclType(VariableDeclAST::param);
    func_stmt->addVariableDeclaration(vdecl);
    m_variableTable.push_back(vdecl->getName());
  }

  VariableDeclAST *var_decl;
  BaseAST *stmt;
  BaseAST *last_stmt;

  // {statement_list}
  if(stmt=visitStatement()) {
    while(stmt) {
      last_stmt = stmt;
      func_stmt->addStatement(stmt);
      stmt=visitStatement();
    }
  } else if(var_decl=visitVariableDeclaration()) {
    while(var_decl) {
      var_decl->setDeclType(VariableDeclAST::local);
      func_stmt->addVariableDeclaration(var_decl);
      m_variableTable.push_back(var_decl->getName());
      var_decl=visitVariableDeclaration();
    }
    
    if(stmt=visitStatement()) {
      while(stmt) {
        last_stmt = stmt;
        func_stmt->addStatement(stmt);
        stmt=visitStatement();
      }
    }
  } else {
    SAFE_DELETE(func_stmt);
    m_tokens->applyTokenIndex(bkup);
    return NULL;
  }

  // "}"
  if(m_tokens->getCurString() == "}") {
    m_tokens->getNextToken();
    return func_stmt;
  } else {
    SAFE_DELETE(func_stmt);
    m_tokens->applyTokenIndex(bkup);
    return NULL;
  }
}

VariableDeclAST *Parser::visitVariableDeclaration()
{
  std::string name;

  // INT
  if(m_tokens->getCurType() == TOK_INT) {
    m_tokens->getNextToken();
  } else {
    return NULL;
  }

  // IDENTIFIER
  if(m_tokens->getCurType() == TOK_IDENTIFIER) {
    name = m_tokens->getCurString();
    m_tokens->getNextToken();
  } else {
    m_tokens->unsetToken(1);
    return NULL;
  }

  // ";"
  if(m_tokens->getCurString() == ";") {
    m_tokens->getNextToken();
    return new VariableDeclAST(name);
  } else {
    m_tokens->unsetToken(2);
    return NULL;
  }
}

BaseAST *Parser::visitStatement()
{
  BaseAST *stmt = NULL;
  if(stmt = visitExpressionStatement()) {
    return stmt;
  } else if(stmt = visitJumpStatement()) {
    return stmt;
  } else {
    return NULL;
  }
}

BaseAST *Parser::visitExpressionStatement()
{
  BaseAST *assign_expr;

  // NULL Expression
  if(m_tokens->getCurString() == ";") {
    m_tokens->getNextToken();
    return new NullExprAST();
  } else if((assign_expr=visitAssignmentExpression())) {
    if(m_tokens->getCurString() == ";") {
      m_tokens->getNextToken();
      return assign_expr;
    }
  }
  return NULL;
}

BaseAST *Parser::visitJumpStatement()
{
  int bkup = m_tokens->getCurIndex();
  BaseAST *expr;

  if(m_tokens->getCurType() == TOK_RETURN) {
    m_tokens->getNextToken();
    if(!(expr=visitAssignmentExpression())) {
      m_tokens->applyTokenIndex(bkup);
      return NULL;
    }
  }
}

BaseAST *Parser::visitAssignmentExpression()
{
  int bkup = m_tokens->getCurIndex();

  BaseAST *lhs;
  if(m_tokens->getCurType() == TOK_IDENTIFIER) {
    lhs = new VariableAST(m_tokens->getCurString());
    m_tokens->getNextToken();
    BaseAST *rhs;

    if(m_tokens->getCurType() == TOK_SYMBOL &&
       m_tokens->getCurString() == "=") {
      m_tokens->getNextToken();
      if(rhs = visitAdditiveExpression(NULL)) {
        return new BinaryExprAST("=", lhs, rhs);
      } else {
        SAFE_DELETE(lhs);
        m_tokens->applyTokenIndex(bkup);
      }
    } else {
      SAFE_DELETE(lhs);
      m_tokens->applyTokenIndex(bkup);
    }
  } else {
    m_tokens->applyTokenIndex(bkup);
  }

  BaseAST *add_expr = visitAdditiveExpression(NULL);
  if(add_expr) {
    return add_expr;
  }
  return NULL;
}

BaseAST *Parser::visitAdditiveExpression(BaseAST *lhs)
{
	BaseAST *rhs;
  int bkup = m_tokens->getCurIndex();

  if(!lhs) {
    return NULL;
  }

  // +
  if(m_tokens->getCurType() == TOK_SYMBOL &&
     m_tokens->getCurString() == "+")
  {
    m_tokens->getNextToken();
    rhs = visitMultiplicativeExpression(NULL);
    if(rhs) {
      return visitAdditiveExpression(new BinaryExprAST("+", lhs, rhs));
    } else {
      SAFE_DELETE(lhs);
      m_tokens->applyTokenIndex(bkup);
      return NULL;
    }
  } else if(m_tokens->getCurType() == TOK_SYMBOL &&
            m_tokens->getCurString() == "-") {
    m_tokens->getNextToken();
    rhs = visitMultiplicativeExpression(NULL);
    if(rhs) {
      return visitAdditiveExpression(new BinaryExprAST("-", lhs, rhs));
    } else {
      SAFE_DELETE(lhs);
      m_tokens->applyTokenIndex(bkup);
      return NULL;
    }
  }
  return lhs;
}

BaseAST *Parser::visitMultiplicativeExpression(BaseAST *lhs)
{
  int bkup = m_tokens->getCurIndex();

  if(!lhs) {
    lhs = visitPostfixExpression();
  }
  BaseAST *rhs;

  if(!lhs) {
    return NULL;
  }

  // *
  if(m_tokens->getCurType() == TOK_SYMBOL &&
     m_tokens->getCurString() == "*")
  {
    m_tokens->getNextToken();
    rhs = visitMultiplicativeExpression(NULL);
    if(rhs) {
      return visitAdditiveExpression(new BinaryExprAST("*", lhs, rhs));
    } else {
      SAFE_DELETE(lhs);
      m_tokens->applyTokenIndex(bkup);
      return NULL;
    }
  } else if(m_tokens->getCurType() == TOK_SYMBOL &&
            m_tokens->getCurString() == "/") {
    m_tokens->getNextToken();
    rhs = visitMultiplicativeExpression(NULL);
    if(rhs) {
      return visitAdditiveExpression(new BinaryExprAST("/", lhs, rhs));
    } else {
      SAFE_DELETE(lhs);
      m_tokens->applyTokenIndex(bkup);
      return NULL;
    }
  }
  return lhs;
}

BaseAST *Parser::visitPostfixExpression() {
  int bkup = m_tokens->getCurIndex();

  BaseAST *prim_expr = visitPrimaryExpression();
  if(prim_expr) {
    return prim_expr;
  }

  if(m_tokens->getCurType() == TOK_IDENTIFIER) {
    int param_num;
    if(m_prototypeTable.find(m_tokens->getCurString()) != m_prototypeTable.end()) {
      param_num = m_prototypeTable[m_tokens->getCurString()];
    } else if(m_functionTable.find(m_tokens->getCurString()) != m_functionTable.end()) {
      param_num = m_functionTable[m_tokens->getCurString()];
    } else {
      return NULL;
    }

    // get function name
    std::string callee = m_tokens->getCurString();
    m_tokens->getNextToken();

    // left paren
    if(m_tokens->getCurType() != TOK_SYMBOL ||
       m_tokens->getCurString() != "(") {
      m_tokens->applyTokenIndex(bkup);
      return NULL;
    }

    m_tokens->getNextToken();

    // argument list
    std::vector<BaseAST*> args;
    BaseAST *assign_expr = visitAssignmentExpression();
    if(assign_expr) {
      args.push_back(assign_expr);
      while(m_tokens->getCurType() == TOK_SYMBOL &&
            m_tokens->getCurString() == ",") {
        m_tokens->getNextToken();

        // IDENTIFIER
        assign_expr = visitAssignmentExpression();
        if(assign_expr) {
          args.push_back(assign_expr);
        } else {
          break;
        }
      }
    }

    // check number of argument
    if(args.size() != param_num) {
      for(int i = 0; i < args.size(); i++) {
        SAFE_DELETE(args[i]);
      }
      m_tokens->applyTokenIndex(bkup);
      return NULL;
    }

    // RIGHT PALEN
    if(m_tokens->getCurType() == TOK_SYMBOL &&
       m_tokens->getCurString() == ")") {
      m_tokens->getNextToken();
      return new CallExprAST(callee, args);
    } else {
      for(int i = 0; i < args.size(); i++) {
        SAFE_DELETE(args[i]);
      }
      m_tokens->applyTokenIndex(bkup);
      return NULL;
    }
  } else {
    return NULL;
  }
}

BaseAST *Parser::visitPrimaryExpression() {
  int bkup = m_tokens->getCurIndex();

  // variable_identifier
  if(m_tokens->getCurType() == TOK_IDENTIFIER &&
     (std::find(m_variableTable.begin(), m_variableTable.end(), m_tokens->getCurString()) !=
      m_variableTable.end()))
  {
    std::string var_name = m_tokens->getCurString();
    m_tokens->getNextToken();
    return new VariableAST(var_name);
  } else if(m_tokens->getCurType() == TOK_DIGIT) { // integer
    int val = m_tokens->getCurNum();
    m_tokens->getNextToken();
    return new NumberAST(val);
  } else if(m_tokens->getCurType() == TOK_SYMBOL &&
            m_tokens->getCurString() == "-") { // integer(-)
    if(m_tokens->getCurType() == TOK_DIGIT) {
      int val = m_tokens->getCurNum();
      m_tokens->getNextToken();
      return new NumberAST(-val);
    } else {
      m_tokens->applyTokenIndex(bkup);
      return NULL;
    }
  } else if(m_tokens->getCurType() == TOK_SYMBOL &&
            m_tokens->getCurString() == "(") {
    m_tokens->getNextToken();

    // expression
    BaseAST *assign_expr;
    if(!(assign_expr = visitAssignmentExpression())) {
      m_tokens->applyTokenIndex(bkup);
      return NULL;
    }

    // RIGHT PALEN
    if(m_tokens->getCurString() == ")") {
      m_tokens->getNextToken();
      return assign_expr;
    } else {
      SAFE_DELETE(assign_expr);
      m_tokens->applyTokenIndex(bkup);
      return NULL;
    }
  }
  return NULL;
}

PrototypeAST *Parser::visitPrototype()
{
  std::string func_name;

  int bkup = m_tokens->getCurIndex();

  if(m_tokens->getCurType() == TOK_INT) {
    m_tokens->getNextToken();
  } else {
    return NULL;
  }

  // IDENTIFIER
  if(m_tokens->getCurType() == TOK_IDENTIFIER) {
    func_name = m_tokens->getCurString();
    m_tokens->getNextToken();
  } else {
    m_tokens->unsetToken(1);
    return NULL;
  }

  // '('
  if(m_tokens->getCurString() == "(") {
    m_tokens->getNextToken();
  } else {
    m_tokens->unsetToken(2);
    return NULL;
  }

  // param_list
  std::vector<std::string> param_list;
  while(true)
  {
    if(m_tokens->getCurType() == TOK_SYMBOL && m_tokens->getCurString() == ",") {
      m_tokens->getNextToken();
    }
    if(m_tokens->getCurType() == TOK_INT) {
      m_tokens->getNextToken();
    } else {
      break;
    }

    if(m_tokens->getCurType() == TOK_IDENTIFIER) {
      if(std::find(param_list.begin(), param_list.end(), m_tokens->getCurString()) != param_list.end()) {
        m_tokens->applyTokenIndex(bkup);
        return NULL;
      }
      param_list.push_back(m_tokens->getCurString());
      m_tokens->getNextToken();
    } else {
      m_tokens->applyTokenIndex(bkup);
      return NULL;
    }
  }

  // ')'
  if(m_tokens->getCurString() == ")") {
    m_tokens->getNextToken();
    return new PrototypeAST(func_name, param_list);
  } else {
    m_tokens->applyTokenIndex(bkup);
    return NULL;
  }
}

void Parser::LexicalAnalysys(std::string filename)
{
  std::fstream fs(filename, std::ios::in);
  std::string str;
  Token *token;

  for(;;) {
    if(fs.eof()) {
      break;
    } else {
      std::string ident;
      bool iden_bool = false;
      getline(fs, str);
      for(int index = 0; index < str.size(); ++index) {
        if(str.at(index) == ' ') {
          if(iden_bool) {
            token = new Token(ident, TOK_IDENTIFIER);
            m_tokens->setToken(token);
            ident = "";
            iden_bool = false;
          }
          continue;
        }
        // return value int
        else if(str.at(index) == 'i' && str.at(index + 1) == 'n' && str.at(index + 2) == 't') {
          if(iden_bool) {
            token = new Token(ident, TOK_IDENTIFIER);
            m_tokens->setToken(token);
            ident = "";
            iden_bool = false;
          }
          token = new Token("int", TOK_INT);
          index += 2;
          m_tokens->setToken(token);
        }
        // comment
        else if(str.at(index) == '/' && str.at(index + 1) == '/') { 
          if(iden_bool) {
            token = new Token(ident, TOK_IDENTIFIER);
            m_tokens->setToken(token);
            ident = "";
            iden_bool = false;
          }
          index = str.size();
        }
        // return
        else if(str.at(index) == 'r' && str.at(index + 1) == 'e'
           && str.at(index + 2) == 't' && str.at(index + 3) == 'u'
           && str.at(index + 4) == 'r' && str.at(index + 5) == 'n')
        {
          if(iden_bool) {
            token = new Token(ident, TOK_IDENTIFIER);
            m_tokens->setToken(token);
            ident = "";
            iden_bool = false;
          }
          token = new Token("return", TOK_RETURN);
          index += 5;
          m_tokens->setToken(token);
        }
        // digit
        else if(isdigit(str.at(index))) {
          if(iden_bool) {
            token = new Token(ident, TOK_IDENTIFIER);
            m_tokens->setToken(token);
            ident = "";
            iden_bool = false;
          }
          int i = 1;
          for(;;i++) {
            if(isdigit(str.at(index + i))) {
              // no 
            } else {
              break;
            }
          }
          token = new Token(str.substr(index, i), TOK_INT);
          m_tokens->setToken(token);
        }
        else if(str.at(index) == '*' || str.at(index) == '+' || str.at(index) == '-' ||
           str.at(index) == '/' || str.at(index) == ';' || str.at(index) == '{' ||
           str.at(index) == '}' || str.at(index) == '(' || str.at(index) == ')' ||
           str.at(index) == '=' )
        {
          if(iden_bool) {
            token = new Token(ident, TOK_IDENTIFIER);
            m_tokens->setToken(token);
            ident = "";
            iden_bool = false;
          }
          token = new Token(str.substr(index, 1), TOK_SYMBOL);
          m_tokens->setToken(token);
        }
        else
        {
          ident += str.substr(index, 1);
          iden_bool = true;
        }
      }
    }
  }

  std::fstream fs2("token.txt", std::ios::out);

  int x = m_tokens->getCurIndex();
  std::string tok_str[] = {
    "TOK_INT",
    "TOK_EOF",
    "TOK_IDENTIFIER",
    "TOK_DIGIT",
    "TOK_SYMBOL",
    "TOK_RETURN",
    "TOK_UNKNOWN",
  };  
  for(int i = 0; i < x; ++i) {
      m_tokens->applyTokenIndex(i);
      fs2 << i << ": " << tok_str[m_tokens->getToken(i)->getType()] << ": " << m_tokens->getToken(i)->getString() << std::endl;
  }

  fs2.close();

  fs.close();
}

