// ---------- [start] my definition ----------

#include "Parser.hpp"

Parser::Parser(std::string filename){
  m_Tokens = LexicalAnalysis(filename);
}

bool Parser::doParse() {
  if(!m_Tokens) {
    fprintf(stderr, "error at lexer\n");
    return false;
  } else {
    return visitTranslationUnit();
  }
}

TranslationUnitAST &Parser::getAST() {
  if(m_TU) {
    return *m_TU;
  } else {
    return *(new TranslationUnitAST());
  }
}

bool Parser::visitTranslationUnit() {
  m_TU = new TranslationUnitAST();
  std::vector<std::string> param_list;
  param_list.push_back("i");
  m_TU->addPrototype(new PrototypeAST("printnum", param_list));
  m_PrototypeTable["printnum"] = 1;

  // ExternalDecl
  while(true) {
    if(!visitExternalDeclaration(m_TU)) {
      SAFE_DELETE(m_TU);
      return false;
    }
    if(m_Tokens->getCurType() == TOK_EOF) {
      break;
    }
  }
  return true;
}


TokenStream *Parser::LexicalAnalysis(std::string filename) {
  TokenStream *tokens = new TokenStream();
  std::fstream ifs;
  std::string cur_line;
  std::string token_str;
  int line_num = 0;
  bool iscomment = false;

  ifs.open(filename.c_str(), std::ios::in);
  if(!ifs) { return NULL; }
  while(ifs && getline(ifs, cur_line)) {
    char next_char;
    std::string line;
    Token *next_token;
    int index = 0;
    int length = cur_line.length();

    while(index < length) {
      next_char = cur_line.at(index++);

      // jump commentout
      if(iscomment) {
        // if last 2 characters are not comment closure, will jump load 2 characters .
        if((length - index) < 2 || (cur_line.at(index) != '*') || (cur_line.at(index++) != '/')) {
          continue;
        } else {
          iscomment = false;
        }
      }

      if(next_char == EOF) { /* EOF */
        token_str = EOF;
        next_token = new Token(token_str, TOK_EOF, line_num);
      } else if(isspace(next_char)) { /* SPACE */
        continue;
      } else if(isalpha(next_char)) { /* IDENTIFIER */
        token_str += next_char;
        next_char = cur_line.at(index++);
        while(isalnum(next_char)) {
          token_str += next_char;
          next_char = cur_line.at(index++);
          if(index == length) { break; }
        }
        index--;

        if(token_str == "int") {
          next_token = new Token(token_str, TOK_INT, line_num);
        } else if(token_str == "return") {
          next_token = new Token(token_str, TOK_RETURN, line_num);
        } else {
          next_token = new Token(token_str, TOK_IDENTIFIER, line_num);
        }
      } else if(isdigit(next_char)) {
        if(next_char == '0') {
          token_str += next_char;
          next_token = new Token(token_str, TOK_DIGIT, line_num);
        } else {
          token_str += next_char;
          next_char = cur_line.at(index++);
          while(isdigit(next_char)) {
            token_str += next_char;
            next_char = cur_line.at(index++);
          }
          next_token = new Token(token_str, TOK_DIGIT, line_num);
          index--;
        }
      } else if(next_char == '/') {
        token_str += next_char;
        next_char = cur_line.at(index++);

        // if comment
        if(next_char == '/') {
          iscomment = true;
          continue;
        } else if(next_char == '*') {
          iscomment = true;
          continue;
        } else { /* DIVIDER('/') */
          index--;
          next_token = new Token(token_str, TOK_SYMBOL, line_num);
        }
      } else { /* OTHER TOKEN */
        if(next_char == '*' || next_char == '+' || next_char == '-' || next_char == '.' || next_char == ',' ||
           next_char == '(' || next_char == ')' || next_char == '{' || next_char == '}' || next_char == ';' ||
           next_char == '=')
        {
          token_str += next_char;
          next_token = new Token(token_str, TOK_SYMBOL, line_num);
        } else { /* This token impossible parse. */
          fprintf(stderr, "unclear token : %c", next_char);
          SAFE_DELETE(tokens);
          return NULL;
        }
      }

      // add Tokens
      tokens->pushToken(next_token);
      token_str.clear();
    }
    token_str.clear();
    line_num++;
  }

  if(ifs.eof()) {
    tokens->pushToken(new Token(token_str, TOK_EOF, line_num));
  }
  ifs.close();
  tokens->printTokens();
  return tokens;
}

// ---------- [end] my definition ----------

/**
  * ExternalDeclaration用構文解析クラス
  * 解析したPrototyupeとFunctionASTをTranslationUitに追加
  * @param TranslationUnitAST
  * @return true 
  */
bool Parser::visitExternalDeclaration(
		TranslationUnitAST *tunit
		){
	//FunctionDeclaration
	PrototypeAST *proto=visitFunctionDeclaration();
	if(proto){
		tunit->addPrototype(proto);
		return true;
	}

	//FunctionDefinition
	FunctionAST *func_def=visitFunctionDefinition();
	if(func_def){
		tunit->addFunction(func_def);
		return true;
	}

	return false;
}


/**
  * FunctionDclaration用構文解析メソッド
  * @return 解析成功：PrototypeAST　解析失敗：NULL
  */
PrototypeAST *Parser::visitFunctionDeclaration(){
	int bkup=m_Tokens->getCurIndex();
	PrototypeAST *proto=visitPrototype();
	if(!proto){
		return NULL;
	}

	//prototype;
	if(m_Tokens->getCurString()==";"){
		if( m_PrototypeTable.find(proto->getName()) != m_PrototypeTable.end() ||
			(m_FunctionTable.find(proto->getName()) != m_FunctionTable.end() &&
			m_FunctionTable[proto->getName()] != proto->getParamNum() ) ){
			fprintf(stderr, "Function：%s is redefined" ,proto->getName().c_str()); 
			SAFE_DELETE(proto);
			return NULL;
		}
		m_PrototypeTable[proto->getName()]=proto->getParamNum();
		m_Tokens->getNextToken();
		return proto;
	}else{
		SAFE_DELETE(proto);
		m_Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	
}


/**
  * FunctionDfinition用構文解析メソッド
  * @return 解析成功：FunctionAST　解析失敗：NULL
  */
FunctionAST *Parser::visitFunctionDefinition(){
	int bkup=m_Tokens->getCurIndex();

	PrototypeAST *proto=visitPrototype();
	if(!proto){
		return NULL;
	}else if( (m_PrototypeTable.find(proto->getName()) != m_PrototypeTable.end() &&
				m_PrototypeTable[proto->getName()] != proto->getParamNum() ) ||
				m_FunctionTable.find(proto->getName()) != m_FunctionTable.end()){
			fprintf(stderr, "Function：%s is redefined" ,proto->getName().c_str()); 
			SAFE_DELETE(proto);
			return NULL;
	}

	m_VariableTable.clear();
	FunctionStatementAST *func_stmt = visitFunctionStatement(proto);
	if(func_stmt){
		m_FunctionTable[proto->getName()]=proto->getParamNum();
		return new FunctionAST(proto,func_stmt);
	}else{
		SAFE_DELETE(proto);
		m_Tokens->applyTokenIndex(bkup);
		return NULL;
	}
}


/**
  * Prototype用構文解析メソッド
  * @return 解析成功：PrototypeAST　解析失敗：NULL
  */
PrototypeAST *Parser	::visitPrototype(){
	std::string func_name;

	//bkup index
	int bkup=m_Tokens->getCurIndex();

	//type_specifier
	if(m_Tokens->getCurType()==TOK_INT){
		m_Tokens->getNextToken();
	}else{
		return NULL;
	}


	//IDENTIFIER
	if(m_Tokens->getCurType()==TOK_IDENTIFIER){
		func_name=m_Tokens->getCurString();
		m_Tokens->getNextToken();
	}else{
		m_Tokens->ungetToken(1);	//unget TOK_INT
		return NULL;
	}

	//'('
	if(m_Tokens->getCurString()=="("){
		m_Tokens->getNextToken();
	}else{
		m_Tokens->ungetToken(2);	//unget TOK_INT IDENTIFIER
		return NULL;
	}


	//parameter_list
	std::vector<std::string> param_list;
	bool is_first_param = true;
	while(true){
		//','
		if(!is_first_param && m_Tokens->getCurType()==TOK_SYMBOL && m_Tokens->getCurString()==","){
			m_Tokens->getNextToken();
		}
		if(m_Tokens->getCurType()==TOK_INT){
			m_Tokens->getNextToken();
		}else{
			break;
		}

		if(m_Tokens->getCurType()==TOK_IDENTIFIER){
			//引数の変数名に被りがないか確認
			if(std::find(param_list.begin(), param_list.end(), m_Tokens->getCurString()) !=	param_list.end()){
				m_Tokens->applyTokenIndex(bkup);
				return NULL;
			}
			param_list.push_back(m_Tokens->getCurString());
			m_Tokens->getNextToken();
		}else{
			m_Tokens->applyTokenIndex(bkup);
			return NULL;
		}
		is_first_param = false;
	}
	

	//')'
	if(m_Tokens->getCurString()==")"){
		m_Tokens->getNextToken();
		return new PrototypeAST(func_name, param_list);
	}else{
		m_Tokens->applyTokenIndex(bkup);
		return NULL;
	}
}


/**
  * FunctionStatement用構文解析メソッド
  * @param 勘数名，引数を格納したPrototypeクラスのインスタンス
  * @return 解析成功：FunctionSmtAST　解析失敗：NULL
  */
FunctionStatementAST *Parser::visitFunctionStatement(PrototypeAST *proto){
	int bkup=m_Tokens->getCurIndex();

	//{
	if(m_Tokens->getCurString()=="{"){
		m_Tokens->getNextToken();
	}else{
		return NULL;
	}

	//create FunctionStatement
	FunctionStatementAST *func_stmt = new FunctionStatementAST();

	//add parameter to FunctionStatement
	for(int i=0; i<proto->getParamNum(); i++){
		VariableDeclarationAST *vdecl=new VariableDeclarationAST(proto->getParamName(i));
		vdecl->setDeclType(VariableDeclarationAST::param);
		func_stmt->addVariableDeclaration(vdecl);
		m_VariableTable.push_back(vdecl->getName());
	}

	VariableDeclarationAST *var_decl;
	BaseAST *stmt;
	BaseAST *last_stmt;

	//{statement_list}
	if(stmt=(BaseAST*)visitStatement()){
		while(stmt){
			last_stmt=stmt;
			func_stmt->addStatement(stmt);
			stmt=(BaseAST*)visitStatement();
		}

	//variable_declaration_list
	}else if(var_decl=visitVariableDeclaration()){
		while(var_decl){
			var_decl->setDeclType(VariableDeclarationAST::local);
			if(std::find(m_VariableTable.begin(), m_VariableTable.end(), var_decl->getName()) !=
					m_VariableTable.end()){
				SAFE_DELETE(var_decl);
				SAFE_DELETE(func_stmt);
				return NULL;
			}
			func_stmt->addVariableDeclaration(var_decl);
			m_VariableTable.push_back(var_decl->getName());
			//parse Variable Delaration
			var_decl=visitVariableDeclaration();
		}

		if(stmt=(BaseAST*)visitStatement()){
			while(stmt){
				last_stmt=stmt;
				func_stmt->addStatement(stmt);
				stmt=(BaseAST*)visitStatement();
			}
		}

	//other
	}else{
		SAFE_DELETE(func_stmt);
		m_Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	//check if last statement is jump_statement
	if(!last_stmt || !llvm::isa<JumpStatementAST>(last_stmt)){
		SAFE_DELETE(func_stmt);
		m_Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	//}
	if(m_Tokens->getCurString()=="}"){
		m_Tokens->getNextToken();
		return func_stmt;
	}else{
		SAFE_DELETE(func_stmt);
		m_Tokens->applyTokenIndex(bkup);
		return NULL;
	}

}


/**
  * VariableDeclaration用構文解析メソッド
  * @return 解析成功：VariableDeclarationAST　解析失敗：NULL
  */
VariableDeclarationAST *Parser::visitVariableDeclaration(){
	std::string name;

	//INT
	if(m_Tokens->getCurType()==TOK_INT){
		m_Tokens->getNextToken();	
	}else{
		return NULL;
	}
	
	//IDENTIFIER
	if(m_Tokens->getCurType()==TOK_IDENTIFIER){
		name=m_Tokens->getCurString();
		m_Tokens->getNextToken();
	}else{
		m_Tokens->ungetToken(1);
		return NULL;
	}
	
	//';'
	if(m_Tokens->getCurString()==";"){
		m_Tokens->getNextToken();
		return new VariableDeclarationAST(name);
	}else{
		m_Tokens->ungetToken(2);	
		return NULL;
	}
}


/**
  * Statement用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitStatement(){
	BaseAST *stmt=NULL;
	if(stmt=visitExpressionStatement()){
		return stmt;
	}else if(stmt=visitJumpStatement()){
		return stmt;
	}else{
		return NULL;
	}
}


/**
  * ExpressionStatement用構文解析メソッド
  * @return  解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitExpressionStatement(){
	BaseAST *assign_expr;

	//NULL Expression
	if(m_Tokens->getCurString()==";"){
		m_Tokens->getNextToken();
		return new NullExprAST();
	}else if((assign_expr=visitAssignmentExpression())){
		if(m_Tokens->getCurString()==";"){
			m_Tokens->getNextToken();
			return assign_expr;
		}
	}
	return NULL;
}


/**
  * JumpStatement用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitJumpStatement(){
	//bakup index
	int bkup=m_Tokens->getCurIndex();
	BaseAST *expr;

	if(m_Tokens->getCurType() == TOK_RETURN){
		m_Tokens->getNextToken();	
		if(!(expr=visitAssignmentExpression()) ){
			m_Tokens->applyTokenIndex(bkup);
			return NULL;
		}

		if(m_Tokens->getCurString()==";"){
			m_Tokens->getNextToken();
			return new JumpStatementAST(expr);
		}else{
			m_Tokens->applyTokenIndex(bkup);
			return NULL;
		}
	}else{
		return NULL;
	}
}


/**
  * AssignmentExpression用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitAssignmentExpression(){
	int bkup=m_Tokens->getCurIndex();

	//	| IDENTIFIER '=' additive_expression
	BaseAST *lhs;
	if(m_Tokens->getCurType()==TOK_IDENTIFIER){
		//変数が宣言されているか確認
		if(std::find(m_VariableTable.begin(), m_VariableTable.end(), m_Tokens->getCurString()) !=
				m_VariableTable.end()){

			lhs=new VariableAST(m_Tokens->getCurString());
			m_Tokens->getNextToken();
			BaseAST *rhs;
			if(m_Tokens->getCurType()==TOK_SYMBOL &&
				m_Tokens->getCurString()=="="){
				m_Tokens->getNextToken();
				if(rhs=visitAdditiveExpression(NULL)){
					return new BinaryExprAST("=", lhs, rhs);
				}else{
					SAFE_DELETE(lhs);
					m_Tokens->applyTokenIndex(bkup);
				}
			}else{
				SAFE_DELETE(lhs);
				m_Tokens->applyTokenIndex(bkup);
			}
		}else{
			m_Tokens->applyTokenIndex(bkup);
		}
	}

	//additive_expression
	BaseAST *add_expr=visitAdditiveExpression(NULL);
	if(add_expr){
		return add_expr;
	}

	return NULL;
}



/**
  * AdditiveExpression用構文解析メソッド
  * @param lhs(左辺),初回呼び出し時はNULL
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitAdditiveExpression(BaseAST *lhs){
	//bkup index
	int bkup=m_Tokens->getCurIndex();

	if(!lhs)
		lhs=visitMultiplicativeExpression(NULL);
	BaseAST *rhs;

	if(!lhs){
		return NULL;
	}
	//+
	if(m_Tokens->getCurType()==TOK_SYMBOL &&
				m_Tokens->getCurString()=="+"){
		m_Tokens->getNextToken();
		rhs=visitMultiplicativeExpression(NULL);
		if(rhs){
			return visitAdditiveExpression(
						new BinaryExprAST("+", lhs, rhs)
					);
		}else{
			SAFE_DELETE(lhs);
			m_Tokens->applyTokenIndex(bkup);
			return NULL;
		}
			
	//-
	}else if(m_Tokens->getCurType()==TOK_SYMBOL &&
				m_Tokens->getCurString()=="-"){
		m_Tokens->getNextToken();
		rhs=visitMultiplicativeExpression(NULL);
		if(rhs){
			return visitAdditiveExpression(
						new BinaryExprAST("-", lhs, rhs)
					);
		}else{
			SAFE_DELETE(lhs);
			m_Tokens->applyTokenIndex(bkup);
			return NULL;
		}	
	}	
	return lhs;
}


/**
  * MultiplicativeExpression用解析メソッド
  * @param  lhs(左辺),初回呼び出し時はNULL
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitMultiplicativeExpression(BaseAST *lhs){
	//bkup index
	int bkup=m_Tokens->getCurIndex();

	//BaseAST *lhs=visitPostfixExpression();
	if(!lhs)
		lhs=visitPostfixExpression();
	BaseAST *rhs;

	if(!lhs){
			return NULL;
	}
	// *
	if(m_Tokens->getCurType()==TOK_SYMBOL &&
				m_Tokens->getCurString()=="*"){
		m_Tokens->getNextToken();
		rhs=visitPostfixExpression();
		if(rhs){
			return visitMultiplicativeExpression(
						new BinaryExprAST("*", lhs, rhs)
					);
		}else{
			SAFE_DELETE(lhs);
			m_Tokens->applyTokenIndex(bkup);
			return NULL;
		}
			
	// /
	}else if(m_Tokens->getCurType()==TOK_SYMBOL &&
				m_Tokens->getCurString()=="/"){
		m_Tokens->getNextToken();
		rhs=visitPostfixExpression();
		if(rhs){
			return visitMultiplicativeExpression(
						new BinaryExprAST("/", lhs, rhs)
					);
		}else{
			SAFE_DELETE(lhs);
			m_Tokens->applyTokenIndex(bkup);
			return NULL;
		}	
	}	
	return lhs;
}


/**
  * PostfixExpression用構文解析メソッド
  * @return 解析成功：AST　解析失敗：NULL
  */
BaseAST *Parser::visitPostfixExpression(){
	//get index
	int bkup=m_Tokens->getCurIndex();

	//primary_expression
	BaseAST *prim_expr=visitPrimaryExpression();
	if(prim_expr){
		return prim_expr;
	}

	//FUNCTION_IDENTIFIER
	if(m_Tokens->getCurType()==TOK_IDENTIFIER){
		//is FUNCTION_IDENTIFIER
		int param_num;
		if(m_PrototypeTable.find(m_Tokens->getCurString()) !=
			m_PrototypeTable.end() ){
			param_num=m_PrototypeTable[m_Tokens->getCurString()];
		}else if(m_FunctionTable.find(m_Tokens->getCurString()) !=
			m_FunctionTable.end()){
			param_num=m_FunctionTable[m_Tokens->getCurString()];
		}else{
			return NULL;
		}

		//関数名取得
		std::string Callee=m_Tokens->getCurString();
		m_Tokens->getNextToken();

		//LEFT PALEN
		if(m_Tokens->getCurType()!=TOK_SYMBOL ||
				m_Tokens->getCurString()!="("){
			m_Tokens->applyTokenIndex(bkup);
			return NULL;
		}

		m_Tokens->getNextToken();
		//argument list
		std::vector<BaseAST*> args;
		BaseAST *assign_expr=visitAssignmentExpression();
		if(assign_expr){
			args.push_back(assign_expr);
			while(m_Tokens->getCurType()==TOK_SYMBOL &&
					m_Tokens->getCurString()==","){
				m_Tokens->getNextToken();

				//IDENTIFIER
				assign_expr=visitAssignmentExpression();
				if(assign_expr){
					args.push_back(assign_expr);
				}else{
					break;
				}
			}//end while
		}

		//引数の数を確認
		if(args.size() != param_num){
			for(int i=0;i<args.size();i++)
				SAFE_DELETE(args[i]);
			m_Tokens->applyTokenIndex(bkup);
			return NULL;
		}
			
		//RIGHT PALEN
		if(m_Tokens->getCurType()==TOK_SYMBOL &&
					m_Tokens->getCurString()==")"){
			m_Tokens->getNextToken();
			return new CallExprAST(Callee, args);
		}else{
			for(int i=0;i<args.size();i++)
				SAFE_DELETE(args[i]);
			m_Tokens->applyTokenIndex(bkup);
			return NULL;
		}

	}else{
		return NULL;
	}
}


/**
  * PrimaryExpression用構文解析メソッド
  * @return 解析成功時：AST　失敗時：NULL
  */
BaseAST *Parser::visitPrimaryExpression(){
	//recored index
	int bkup=m_Tokens->getCurIndex();


	//VARIABLE_IDENTIFIER
	if(m_Tokens->getCurType()==TOK_IDENTIFIER &&
		(std::find(m_VariableTable.begin(), m_VariableTable.end(), m_Tokens->getCurString()) !=
		m_VariableTable.end()) ){
		std::string var_name=m_Tokens->getCurString();
		m_Tokens->getNextToken();
		return new VariableAST(var_name);

	//integer
	}else if(m_Tokens->getCurType()==TOK_DIGIT){
		int val=m_Tokens->getCurNumVal();
		m_Tokens->getNextToken();
		return new NumberAST(val);

	//integer(-)
	}else if(m_Tokens->getCurType()==TOK_SYMBOL &&
			m_Tokens->getCurString()=="-"){
		m_Tokens->getNextToken();
		if(m_Tokens->getCurType()==TOK_DIGIT){
			int val=m_Tokens->getCurNumVal();
			m_Tokens->getNextToken();
			return new NumberAST(-val);
		}else{
			m_Tokens->applyTokenIndex(bkup);
			return NULL;
		}

	// '(' expression ')'
	}else if(m_Tokens->getCurType()==TOK_SYMBOL &&
			m_Tokens->getCurString()=="("){
		m_Tokens->getNextToken();

		//expression
		BaseAST *assign_expr;
		if(!(assign_expr=visitAssignmentExpression())){
			m_Tokens->applyTokenIndex(bkup);
			return NULL;
		}

		//RIGHT PALEN
		if(m_Tokens->getCurString()==")"){
			m_Tokens->getNextToken();
			return assign_expr;
		}else{
			SAFE_DELETE(assign_expr);
			m_Tokens->applyTokenIndex(bkup);
			return NULL;
		}

	}

	return NULL;
}

