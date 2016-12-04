#include "CodeGen.hpp"

CodeGen::CodeGen()
{
  m_builder = new llvm::IRBuilder<>(llvm::getGlobalContext());
  m_mod = NULL;
}

CodeGen::~CodeGen()
{
  SAFE_DELETE(m_builder);
  SAFE_DELETE(m_mod);
}

bool CodeGen::doCodeGen(TranslationUnitAST &tunit, std::string name)
{
  return generateTranslationUnit(tunit, name);
}

llvm::Module &CodeGen::getModule()
{
  if(m_mod) {
    return *m_mod;
  } else {
    return *(new llvm::Module("null", llvm::getGlobalContext()));
  }
}

bool CodeGen::generateTranslationUnit(TranslationUnitAST &tunit, std::string name)
{
  m_mod = new llvm::Module(name, llvm::getGlobalContext());

  // function declaration
  for(int i = 0;; i++) {
    PrototypeAST *proto = tunit.getPrototype(i);
    if(!proto) {
      break;
    } else if(!generatePrototype(proto, m_mod)) {
      SAFE_DELETE(m_mod);
      return false;
    }
  }

  // function definition
  for(int i = 0;;i++) {
    FunctionAST *func = tunit.getFunction(i);
    if(!func) {
      break;
    } else if(!generateFunctionDefinition(func, m_mod)) {
      SAFE_DELETE(m_mod);
      return false;
    }
  }

  return true;
}

llvm::Function *CodeGen::generatePrototype(PrototypeAST *proto, llvm::Module *mod)
{
  llvm::Function *func = mod->getFunction(proto->getName());
  if(func) {
    if(func->arg_size() == proto->getParamNum() && func->empty()) {
      return func;
    } else {
      fprintf(stderr, "error:: function %s is redefined",
              proto->getName().c_str());
      return NULL;
    }
  } 

  std::vector<llvm::Type *> int_types(proto->getParamNum(),
                                      llvm::Type::getInt32Ty(llvm::getGlobalContext()));
  llvm::FunctionType *func_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(llvm::getGlobalContext()), int_types, false);
  func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage,
                          proto->getName(), mod);
  llvm::Function::arg_iterator arg_iter = func->arg_begin();
  for(int i = 0; i < proto->getParamNum(); i++) {
    arg_iter->setName(proto->getParamName(i).append("_arg"));
    arg_iter++;
  }

  return func;
}

llvm::Function *CodeGen::generateFunctionDefinition(FunctionAST *func_ast,
                                                    llvm::Module *mod)
{
  llvm::Function *func = generatePrototype(func_ast->getPrototype(), mod);
  if(!func) {
    return NULL;
  }

  m_curFunc = func;
  llvm::BasicBlock *bblock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", func);
  m_builder->SetInsertPoint(bblock);
  generateFunctionStatement(func_ast->getBody());

  return func;
}

llvm::Value *CodeGen::generateFunctionStatement(FunctionStmtAST *func_stmt)
{
  VariableDeclAST *vdecl;
  llvm::Value *v = NULL;

  for(int i = 0;; i++) {
    if(!func_stmt->getVariableDecl(i)) {
      break;
    }
    vdecl = llvm::dyn_cast<VariableDeclAST>(func_stmt->getVariableDecl(i));
    v = generateVariableDeclaration(vdecl);
  }

  BaseAST *stmt;
  for(int i = 0;; i++) {
    stmt=func_stmt->getStatement(i);
    if(!stmt) {
      break;
    } else if(!llvm::isa<NullExprAST>(stmt)) {
      v = generateStatement(stmt);
    }
    return v;
  }
}

llvm::Value *CodeGen::generateVariableDeclaration(VariableDeclAST *vdecl) {
  llvm::AllocaInst *alloca = m_builder->CreateAlloca(
    llvm::Type::getInt32Ty(llvm::getGlobalContext()), 0, vdecl->getName());

  if(vdecl->getType() == VariableDeclAST::param) {
		llvm::ValueSymbolTable &vs_table = m_curFunc->getValueSymbolTable();
    m_builder->CreateStore(vs_table.lookup(vdecl->getName().append("_arg")),
                           alloca);
  }
  return alloca;
}


llvm::Value *CodeGen::generateStatement(BaseAST *stmt)
{
  if(llvm::isa<BinaryExprAST>(stmt)) {
    return generateBinaryExpression(llvm::dyn_cast<BinaryExprAST>(stmt));
  } else if(llvm::isa<CallExprAST>(stmt)) {
    return generateCallExpression(llvm::dyn_cast<CallExprAST>(stmt));
  } else if(llvm::isa<JumpStmtAST>(stmt)) {
    return generateJumpStatement(llvm::dyn_cast<JumpStmtAST>(stmt));
  } else {
    return NULL;
  }
}

llvm::Value *CodeGen::generateBinaryExpression(BinaryExprAST *bin_expr) {
  BaseAST *lhs = bin_expr->getLHS();
  BaseAST *rhs = bin_expr->getRHS();

  llvm::Value *lhs_v;
  llvm::Value *rhs_v;

  if(bin_expr->getOp() == "=") {
    VariableAST *lhs_var = llvm::dyn_cast<VariableAST>(lhs);
    llvm::ValueSymbolTable &vs_table = m_curFunc->getValueSymbolTable();
    lhs_v = vs_table.lookup(lhs_var->getName());
  } else {
    if(llvm::isa<BinaryExprAST>(lhs)) {
      lhs_v = generateBinaryExpression(llvm::dyn_cast<BinaryExprAST>(lhs));
    } else if(llvm::isa<VariableAST>(lhs)) {
      lhs_v = generateVariable(llvm::dyn_cast<VariableAST>(lhs));
    } else if(llvm::isa<NumberAST>(lhs)) {
      NumberAST *num = llvm::dyn_cast<NumberAST>(lhs);
      lhs_v = generateNumber(num->getNumberValue());
    }
  }


  if(llvm::isa<BinaryExprAST>(rhs)) {
    rhs_v = generateBinaryExpression(llvm::dyn_cast<BinaryExprAST>(rhs));
  } else if(llvm::isa<CallExprAST>(rhs)) {
    rhs_v = generateCallExpression(llvm::dyn_cast<CallExprAST>(rhs));
  } else if(llvm::isa<VariableAST>(rhs)) {
    rhs_v = generateVariable(llvm::dyn_cast<VariableAST>(rhs));
  } else if(llvm::isa<NumberAST>(rhs)) {
    NumberAST *num = llvm::dyn_cast<NumberAST>(rhs);
    rhs_v = generateNumber(num->getNumberValue());
  }

  if(bin_expr->getOp() == "=") {
    // store
    return m_builder->CreateStore(rhs_v, lhs_v);
  } else if(bin_expr->getOp() == "+") {
    m_builder->CreateAdd(lhs_v, rhs_v, "add_tmp");
  } else if(bin_expr->getOp() == "-") {
    m_builder->CreateSub(lhs_v, rhs_v, "sub_tmp");
  } else if(bin_expr->getOp() == "*") {
    m_builder->CreateMul(lhs_v, rhs_v, "mul_tmp");
  } else if(bin_expr->getOp() == "/") {
    m_builder->CreateSDiv(lhs_v, rhs_v, "div_tmp");
  }
}

llvm::Value *CodeGen::generateCallExpression(CallExprAST *call_expr) {
  std::vector<llvm::Value*> arg_vec;
  BaseAST *arg;
  llvm::Value *arg_v;
  llvm::ValueSymbolTable &vs_table = m_curFunc->getValueSymbolTable();

  for(int i = 0;;i++) {
    if(!(arg=call_expr->getArgs(i))) {
      break;
    }

    if(llvm::isa<CallExprAST>(arg)) {
      arg_v = generateCallExpression(llvm::dyn_cast<CallExprAST>(arg));
    } else if(llvm::isa<BinaryExprAST>(arg)) {
      BinaryExprAST *bin_expr = llvm::dyn_cast<BinaryExprAST>(arg);
      arg_v = generateBinaryExpression(llvm::dyn_cast<BinaryExprAST>(arg));
      if(bin_expr->getOp() == "=") {
        VariableAST *var = llvm::dyn_cast<VariableAST>(bin_expr->getLHS());
        arg_v=m_builder->CreateLoad(vs_table.lookup(var->getName()), "arg_val");
      }
    } else if(llvm::isa<VariableAST>(arg)) {
      arg_v=generateVariable(llvm::dyn_cast<VariableAST>(arg));
    } else if(llvm::isa<NumberAST>(arg)) {
      NumberAST *num = llvm::dyn_cast<NumberAST>(arg);
      arg_v=generateNumber(num->getNumberValue());
    }
    arg_vec.push_back(arg_v);
  }

  return m_builder->CreateCall(m_mod->getFunction(call_expr->getCallee()),
                               arg_vec, "call_tmp" );
}

llvm::Value *CodeGen::generateJumpStatement(JumpStmtAST *jump_stmt) {
  BaseAST *expr = jump_stmt->getExpr();
  llvm::Value *ret_v;
  if(llvm::isa<BinaryExprAST>(expr)) {
    ret_v = generateBinaryExpression(llvm::dyn_cast<BinaryExprAST>(expr));
  } else if(llvm::isa<VariableAST>(expr)) {
    VariableAST *var = llvm::dyn_cast<VariableAST>(expr);
    ret_v = generateVariable(var);
  } else if(llvm::isa<NumberAST>(expr)) {
    NumberAST *num = llvm::dyn_cast<NumberAST>(expr);
    ret_v = generateNumber(num->getNumberValue());
  }
  m_builder->CreateRet(ret_v);
}

llvm::Value *CodeGen::generateVariable(VariableAST *var)
{
  llvm::ValueSymbolTable &vs_table = m_curFunc->getValueSymbolTable();
  return m_builder->CreateLoad(vs_table.lookup(var->getName()), "var_tmp");
}

llvm::Value *CodeGen::generateNumber(int value) {
  return llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvm::getGlobalContext()), value);
}

bool CodeGen::linkModule(llvm::Module *dest, std::string file_name) {
  llvm::SMDiagnostic err;
  llvm::Module *link_mod = llvm::ParseIRFile(file_name, err, llvm::getGlobalContext());
  if(!link_mod) {
    return false;
  }

  std::string err_msg;
  if(llvm::Linker::LinkModules(dest, link_mod, llvm::Linker::DestroySource, &err_msg)) {
    return false;
  }
  SAFE_DELETE(link_mod);

  return true;
}

