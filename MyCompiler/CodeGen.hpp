#ifndef __CODEGEN_HPP_
#define __CODEGEN_HPP_

#include<cstdio>
#include<cstdlib>
#include<map>
#include<string>
#include<vector>
#include<llvm/ADT/APInt.h>
#include<llvm/Constants.h>
#include<llvm/ExecutionEngine/ExecutionEngine.h>
#include<llvm/ExecutionEngine/JIT.h>
#include<llvm/Linker.h>
#include<llvm/LLVMContext.h>
#include<llvm/Module.h>
#include<llvm/Metadata.h>
#include<llvm/Support/Casting.h>
#include<llvm/IRBuilder.h>
#include<llvm/Support/IRReader.h>
#include<llvm/MDBuilder.h>
#include<llvm/ValueSymbolTable.h>
#include"APP.hpp"
#include"AST.hpp"

class CodeGen
{
private:

  llvm::Function *m_curFunc;
  llvm::Module *m_mod;
  llvm::IRBuilder<> *m_builder;

public:
  CodeGen();
  ~CodeGen();
  bool doCodeGen(TranslationUnitAST &tunit, std::string name);
  llvm::Module &getModule();

private:
  bool generateTranslationUnit(TranslationUnitAST &tunit, std::string name);
  llvm::Function *generateFunctionDefinition(FunctionAST *func, llvm::Module *mod);
  llvm::Function *generatePrototype(PrototypeAST *proto, llvm::Module *mod);
  llvm::Value *generateFunctionStatement(FunctionStmtAST *func_stmt);
  llvm::Value *generateVariableDeclaration(VariableDeclAST *vdecl);
  llvm::Value *generateStatement(BaseAST *stmt);
  llvm::Value *generateBinaryExpression(BinaryExprAST *bin_expr);
  llvm::Value *generateCallExpression(CallExprAST *call_expr);
  llvm::Value *generateJumpStatement(JumpStmtAST *jump_stmt);
  llvm::Value *generateVariable(VariableAST *var);
  llvm::Value *generateNumber(int value);
  bool linkModule(llvm::Module *dest, std::string file_name);
};

#endif /* __CODEGEN_HPP_ */
