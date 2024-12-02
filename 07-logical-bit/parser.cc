#include "parser.h"

/*
prog : stmt*
stmt : decl-stmt | expr-stmt | null-stmt | if-stmt | block-stmt | for-stmt
null-stmt : ";"
decl-stmt : "int" identifier ("," identifier (= expr)?)* ";"
if-stmt : "if" "(" expr ")" stmt ( "else" stmt )?
for-stmt : "for" "(" expr? ";" expr? ";" expr? ")" stmt
        "for" "(" decl-stmt expr? ";" expr? ")" stmt
block-stmt: "{" stmt* "}"
expr-stmt : expr ";"
expr : assign-expr | equal-expr
assign-expr: identifier "=" expr
equal-expr : relational-expr (("==" | "!=") relational-expr)*
relational-expr: add-expr (("<"|">"|"<="|">=") add-expr)*
add-expr : mult-expr (("+" | "-") mult-expr)*
mult-expr : primary-expr (("*" | "/") primary-expr)*
primary-expr : identifier | number | "(" expr ")"
number: ([0-9])+
identifier : (a-zA-Z_)(a-zA-Z0-9_)*

*/

std::shared_ptr<Program> Parser::ParseProgram()
{
    // parser program
    // 循环parser然后当tok为eof其实就是走到最后了然后结束
    // 其实程序就是由很多个表达式组成的因此这个vector就是用来
    // 存储多个表达式的
    std::vector<std::shared_ptr<AstNode>> nodeVec;
    while (tok.tokenType != TokenType::eof)
    {
        auto stmt = ParseStmt();
        if (stmt)
        {
            nodeVec.push_back(stmt);
        }
    }
    // 这里其实用的是智能指针的拷贝构造函数
    // 用move将nodeVec直接拷贝过去
    auto program = std::make_shared<Program>();
    program->nodeVec = std::move(nodeVec);
    return program;
}

// 语句分发，其实所有语句都是从此而来的
std::shared_ptr<AstNode> Parser::ParseStmt()
{
    /// 处理null语句
    // 是分号就找到下一位然后继续即可，表明当前的expr结束了
    if (tok.tokenType == TokenType::semi)
    {
        Advance();
        return nullptr;
    }
    /// 处理声明语句
    // 遇到int就是变量声明了
    if (IsTypeName())
    {
        return ParserDeclStmt();
    }
    // if stmt
    else if (tok.tokenType == TokenType::kw_if)
    {
        return ParseIfStmt();
    }
    // block stmt
    else if (tok.tokenType == TokenType::l_brace)
    {
        return ParseBlockStmt();
    }
    // for stmt
    else if (tok.tokenType == TokenType::kw_for)
    {
        return ParseForStmt();
    }
    // break stmt
    else if (tok.tokenType == TokenType::kw_break)
    {
        return ParseBreakStmt();
    }
    // continue stmt
    else if (tok.tokenType == TokenType::kw_continue)
    {
        return ParseContinueStmt();
    }
    else
    {
        /// 处理表达式语句
        // parser 一个expr然后加入到数组里
        return ParseExprStmt();
    }
}

std::shared_ptr<AstNode> Parser::ParserDeclStmt()
{
    // parser 声明语句
    // 先直接消耗int
    Consume(TokenType::kw_int);
    CType *baseTy = CType::GetIntTy();

    auto decl = std::make_shared<DeclStmt>();
    /// int a ,b = 3;
    // a, b = 3;
    int i = 0;
    // 没有遇到；就一直parser
    while (tok.tokenType != TokenType::semi)
    {
        if (i++ > 0)
        {
            assert(Consume(TokenType::comma));
        }
        Token temp = tok;
        // auto varibale_name = tok.content;
        //  变量声明的节点
        //  int a = 3; => int a; a = 3;
        //  语义分析
        auto variableDecl = sema.SemaVariableDeclNode(temp, baseTy);
        decl->nodeVec.push_back(variableDecl);

        Consume(TokenType::identifier);
        // 遇到了=就是要赋值
        if (tok.tokenType == TokenType::equal)
        {
            Token optoken = tok;
            Advance();
            auto left = sema.SemaVariableAccessNode(temp);
            // a = 3;
            auto right = ParseExpr();
            // 语义分析
            auto assignExpr = sema.SemaAssignExprNode(left, right, optoken);
            decl->nodeVec.push_back(assignExpr);
        }
    }
    Consume(TokenType::semi);
    return decl;
}

std::shared_ptr<AstNode> Parser::ParseIfStmt()
{
    // If语句
    // if-stmt : "if" "(" expr ")" stmt ( "else" stmt )?

    Consume(TokenType::kw_if);
    Consume(TokenType::l_parent);
    auto condExpr = ParseExpr();
    Consume(TokenType::r_parent);
    auto thenstmt = ParseStmt();
    std::shared_ptr<AstNode> elseStmt = nullptr;
    // 判断是否是else
    if (tok.tokenType == TokenType::kw_else)
    {
        Consume(TokenType::kw_else);
        elseStmt = ParseStmt();
    }
    return sema.SemaIfStmtNode(condExpr, thenstmt, elseStmt);
}

std::shared_ptr<AstNode> Parser::ParseBlockStmt()
{
    sema.EnterScope();
    auto blockStmt = std::make_shared<BlockStmt>();
    Consume(TokenType::l_brace);
    while (tok.tokenType != TokenType::r_brace)
    {
        blockStmt->nodeVec.push_back(ParseStmt());
    }
    Consume(TokenType::r_brace);
    sema.ExitScope();

    return blockStmt;
}

std::shared_ptr<AstNode> Parser::ParseForStmt()
{
    Consume(TokenType::kw_for);
    Consume(TokenType::l_parent);
    // 这里要新开一个环境因为for
    sema.EnterScope();
    auto node = std::make_shared<ForStmt>();

    // 存储break和continue是为了说明只有在for和switch内部才能使用break和continue
    breakNodes.push_back(node);
    continueNodes.push_back(node);

    std::shared_ptr<AstNode> initNode = nullptr;
    std::shared_ptr<AstNode> condNode = nullptr;
    std::shared_ptr<AstNode> incNode = nullptr;
    std::shared_ptr<AstNode> bodyNode = nullptr;

    if (IsTypeName())
    {
        initNode = ParserDeclStmt();
    }
    else
    {
        if (tok.tokenType != TokenType::semi)
        {
            initNode = ParseExpr();
        }
        Consume(TokenType::semi);
    }

    if (tok.tokenType != TokenType::semi)
    {
        condNode = ParseExpr();
    }
    Consume(TokenType::semi);
    if (tok.tokenType != TokenType::r_parent)
    {
        incNode = ParseExpr();
    }
    Consume(TokenType::r_parent);
    bodyNode = ParseStmt();

    node->initNode = initNode;
    node->condNode = condNode;
    node->incNode = incNode;
    node->bodyNode = bodyNode;

    // 循环结束要把break，continue中的节点pop掉
    breakNodes.pop_back();
    continueNodes.pop_back();

    sema.ExitScope();

    return node;
}

std::shared_ptr<AstNode> Parser::ParseBreakStmt()
{
    if (breakNodes.size() == 0)
    {
        GetDiagEngine().Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::err_break_stmt);
    }
    Consume(TokenType::kw_break);
    Consume(TokenType::semi);
    auto node = std::make_shared<BreakStmt>();
    // target需要记录下来
    node->target = breakNodes.back();
    return node;
}

std::shared_ptr<AstNode> Parser::ParseContinueStmt()
{
    if (continueNodes.size() == 0)
    {
        GetDiagEngine().Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::err_continue_stmt);
    }
    Consume(TokenType::kw_continue);
    Consume(TokenType::semi);
    auto node = std::make_shared<ContinueStmt>();
    // target需要记录下来
    node->target = continueNodes.back();
    return node;
}

std::shared_ptr<AstNode> Parser::ParseExprStmt()
{
    // 表达式语句
    auto expr = ParseExpr();
    Consume(TokenType::semi);
    return expr;
}

std::shared_ptr<AstNode> Parser::ParseExpr()
{
    // 左结合
    /*
    expr : assign-expr | add-expr
    assign-expr: identifier "=" expr
    add-expr : mult-expr (("+" | "-") mult-expr)*
     */
    /// 往前看两个字符才能区分
    // LL(n) 分析向前看n个字符
    bool isAssignExpr = false;

    // 判断是否是赋值表达式，可以构建函数
    lexer.SaveState();
    if (tok.tokenType == TokenType::identifier)
    {
        Token temp;
        lexer.NextToken(temp);
        if (temp.tokenType == TokenType::equal)
        {
            isAssignExpr = true;
        }
    }
    lexer.RestoreState();

    // parser 赋值表达式
    // assign-expr: identifier ("=" expr)+
    // 可以是 a=b=c=5;
    if (isAssignExpr)
    {
        return ParseAssignExpr();
    }

    return ParseEqualExpr();
};

std::shared_ptr<AstNode> Parser::ParseEqualExpr()
{
    auto left = ParseRelationalExpr();
    while (tok.tokenType == TokenType::equal_equal ||
           tok.tokenType == TokenType::not_equal)
    {
        OpCode op;
        if (tok.tokenType == TokenType::equal_equal)
        {
            op = OpCode::equal_equal;
        }
        else
        {
            op = OpCode::not_equal;
        }
        Advance();
        auto right = ParseRelationalExpr();
        auto binaryExpr = sema.SemaBinaryExprNode(left, right, op);
        left = binaryExpr;
    }

    return left;
}

std::shared_ptr<AstNode> Parser::ParseRelationalExpr()
{

    auto left = ParseAddExpr();
    while (tok.tokenType == TokenType::less ||
           tok.tokenType == TokenType::less_equal ||
           tok.tokenType == TokenType::greater ||
           tok.tokenType == TokenType::greater_equal)
    {
        OpCode op;
        if (tok.tokenType == TokenType::less)
        {
            op = OpCode::less;
        }
        else if (tok.tokenType == TokenType::less_equal)
        {
            op = OpCode::less_equal;
        }
        else if (tok.tokenType == TokenType::greater)
        {
            op = OpCode::greater;
        }
        else if (tok.tokenType == TokenType::greater_equal)
        {
            op = OpCode::greater_equal;
        }
        Advance();
        auto right = ParseAddExpr();
        auto binaryExpr = sema.SemaBinaryExprNode(left, right, op);
        left = binaryExpr;
    }

    return left;
}

std::shared_ptr<AstNode> Parser::ParseAddExpr()
{

    // parser 加法表达式的分支
    // 左结合就是用一个while循环解决的
    // 碰到 +，- 就看右边 因为左边是term右边也是一个term
    // add-expr : mult-expr (("+" | "-") mult-expr)*
    auto left = ParseMultiExpr();
    while (tok.tokenType == TokenType::plus || tok.tokenType == TokenType::minus)
    {
        OpCode op;
        // 根据tok的值给op赋值
        if (tok.tokenType == TokenType::plus)
        {
            op = OpCode::add;
        }
        else
        {
            op = OpCode::sub;
        }
        // 看下一个也就是+，-右边挨着的
        Advance();
        // 构造一个二元expr的指针
        auto right = ParseMultiExpr();
        auto binaryExpr = sema.SemaBinaryExprNode(left, right, op);
        left = binaryExpr;
        // 最后更新left
        // 看看右孩子得右孩子还有没有了
    }

    return left;
}

std::shared_ptr<AstNode> Parser::ParseMultiExpr()
{

    // 算符优先策略，优化策略
    // factor和term在enbf中非常类似所以这里代码几乎一样
    auto left = ParsePrimary();
    while (tok.tokenType == TokenType::star || tok.tokenType == TokenType::slash)
    {
        OpCode op;
        if (tok.tokenType == TokenType::star)
        {
            op = OpCode::mul;
        }
        else
        {
            op = OpCode::div;
        }
        Advance();
        auto right = ParsePrimary();
        auto binaryExpr = sema.SemaBinaryExprNode(left, right, op);

        left = binaryExpr;
    }

    return left;
};

std::shared_ptr<AstNode> Parser::ParsePrimary()
{
    // 最后就是看终结符
    // 左括号得话里面就又是一个expr
    if (tok.tokenType == TokenType::l_parent)
    {
        Advance();
        auto expr = ParseExpr();
        // 直接用assert了错误直接退出了
        // 因为错误证明括号不匹配了
        assert(Expect(TokenType::r_parent));
        Advance();
        return expr;
    }
    // 有可能最后是标识符
    else if (tok.tokenType == TokenType::identifier)
    {
        auto expr = sema.SemaVariableAccessNode(tok);
        Advance();
        return expr;
    }
    else
    {
        Expect(TokenType::number);
        // 这种情况是number
        auto factor = sema.SemaNumberExprNode(tok, tok.ty);
        Advance();
        return factor;
    }
};

// parser 赋值表达式
// a=b=c=3;
std::shared_ptr<AstNode> Parser::ParseAssignExpr()
{
    Expect(TokenType::identifier);
    // llvm::StringRef text = tok.content;
    Token temp = tok;
    Advance();
    auto expr = sema.SemaVariableAccessNode(temp);
    Token optoken = tok;
    Consume(TokenType::equal);
    return sema.SemaAssignExprNode(expr, ParseExpr(), optoken);
}

bool Parser::Expect(TokenType tokenType)
{
    if (tok.tokenType == tokenType)
    {
        return true;
    }
    GetDiagEngine().Report(
        llvm::SMLoc::getFromPointer(tok.ptr),
        diag::err_expected,
        Token::GetSpellingText(tokenType),
        llvm::StringRef(tok.ptr, tok.len));
    return false;
}

bool Parser::Consume(TokenType tokenType)
{
    if (Expect(tokenType))
    {
        Advance();
        return true;
    }
    return false;
}

void Parser::Advance()
{
    lexer.NextToken(tok);
}

bool Parser::IsTypeName()
{
    if (tok.tokenType == TokenType::kw_int)
    {
        return true;
    }
    return false;
}