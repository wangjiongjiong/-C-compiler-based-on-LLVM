#include "parser.h"

/*
prog : stmt*
stmt : decl-stmt | expr-stmt | null-stmt
null-stmt : ";"
decl-stmt : "int" identifier ("," identifier (= expr)?)* ";"
expr-stmt : expr ";"
expr : assign-expr | add-expr
assign-expr: identifier "=" expr
add-expr : mult-expr (("+" | "-") mult-expr)*
mult-expr : primary-expr (("*" | "/") primary-expr)*
primary-expr : identifier | number | "(" expr ")"
number: ([0-9])+
identifier : (a-zA-Z_)(a-zA-Z0-9_)*
*/

// parser program
std::shared_ptr<Program> Parser::ParseProgram()
{
    // 循环parser然后当tok为eof其实就是走到最后了然后结束
    // 其实程序就是由很多个表达式组成的因此这个vector就是用来
    // 存储多个表达式的
    std::vector<std::shared_ptr<AstNode>> exprVec;
    while (tok.tokenType != TokenType::eof)
    {
        /// 处理null语句

        // 是分号就找到下一位然后继续即可，表明当前的expr结束了
        if (tok.tokenType == TokenType::semi)
        {
            Advance();
            continue;
        }
        /// 处理声明语句
        // 遇到int就是变量声明了
        if (tok.tokenType == TokenType::kw_int)
        {
            const auto &exprs = ParserDeclStmt();
            for (auto &expr : exprs)
            {
                exprVec.push_back(expr);
            }
        }
        else
        {
            /// 处理表达式语句
            // parser 一个expr然后加入到数组里
            auto expr = ParseExprStmt();
            exprVec.push_back(expr);
        }
    }
    // 这里其实用的是智能指针的拷贝构造函数
    // 用move将exprVec直接拷贝过去
    auto program = std::make_shared<Program>();
    program->expVec = std::move(exprVec);
    return program;
}

// parser 声明语句
std::vector<std::shared_ptr<AstNode>> Parser::ParserDeclStmt()
{
    // 先直接消耗int
    Consume(TokenType::kw_int);
    CType *baseTy = CType::GetIntTy();

    // 最后直接存到vector中
    std::vector<std::shared_ptr<AstNode>> astArr;
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
        astArr.push_back(variableDecl);

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
            astArr.push_back(assignExpr);
        }
    }
    Consume(TokenType::semi);
    return astArr;
}

// 表达式语句
std::shared_ptr<AstNode> Parser::ParseExprStmt()
{
    auto expr = ParseExpr();
    Consume(TokenType::semi);
    return expr;
}

// 左结合
/*
expr : assign-expr | add-expr
assign-expr: identifier "=" expr
add-expr : mult-expr (("+" | "-") mult-expr)*
 */
/// 往前看两个字符才能区分
// LL(n) 分析向前看n个字符
std::shared_ptr<AstNode> Parser::ParseExpr()
{
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

    // parser 加法表达式的分支
    // 左结合就是用一个while循环解决的
    // 碰到 +，- 就看右边 因为左边是term右边也是一个term
    // add-expr : mult-expr (("+" | "-") mult-expr)*
    auto left = ParseTerm();
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
        auto right = ParseTerm();
        auto binaryExpr = sema.SemaBinaryExprNode(left, right, op);
        left = binaryExpr;
        // 最后更新left
        // 看看右孩子得右孩子还有没有了
    }

    return left;
};

// 算符优先策略，优化策略
// factor和term在enbf中非常类似所以这里代码几乎一样
std::shared_ptr<AstNode> Parser::ParseTerm()
{

    auto left = ParseFactor();
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
        auto right = ParseFactor();
        auto binaryExpr = sema.SemaBinaryExprNode(left, right, op);

        left = binaryExpr;
    }

    return left;
};
// 最后就是看终结符
std::shared_ptr<AstNode> Parser::ParseFactor()
{
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