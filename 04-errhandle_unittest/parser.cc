#include "parser.h"

/*
prog : (expr? ";")*
expr : term (("+" | "-") trem)* ;
term : factor (("*" | "/") factor)* ;
factor : number | "(" expr ")" ;
number : ([0-9])+
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
        // 是分号就找到下一位然后继续即可，表明当前的expr结束了
        if (tok.tokenType == TokenType::semi)
        {
            Advance();
            continue;
        }
        // 遇到int就是变量声明了
        if (tok.tokenType == TokenType::kw_int)
        {
            const auto &exprs = ParserDecl();
            for (auto &expr : exprs)
            {
                exprVec.push_back(expr);
            }
        }
        else
        {
            // parser 一个expr然后加入到数组里
            auto expr = ParseExpr();
            exprVec.push_back(expr);
        }
    }
    // 这里其实用的是智能指针的拷贝构造函数
    // 用move将exprVec直接拷贝过去
    auto program = std::make_shared<Program>();
    program->expVec = std::move(exprVec);
    return program;
}

// parser 声明
std::vector<std::shared_ptr<AstNode>> Parser::ParserDecl()
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
        auto varibale_name = tok.content;
        // 变量声明的节点
        // int a = 3; => int a; a = 3;
        // 语义分析
        auto variableDecl = sema.SemaVariableDeclNode(varibale_name, baseTy);
        astArr.push_back(variableDecl);

        Consume(TokenType::identifier);
        // 遇到了=就是要赋值
        if (tok.tokenType == TokenType::equal)
        {

            Advance();
            auto left = sema.SemaVariableAccessNode(varibale_name);
            // a = 3;
            auto right = ParseExpr();
            // 语义分析
            auto assignExpr = sema.SemaAssignExprNode(left, right);
            astArr.push_back(assignExpr);
        }
    }
    Consume(TokenType::semi);
    return astArr;
}

// 左结合
std::shared_ptr<AstNode> Parser::ParseExpr()
{
    // 左结合就是用一个while循环解决的
    // 碰到 +，- 就看右边 因为左边是term右边也是一个term
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
        auto expr = sema.SemaVariableAccessNode(tok.content);
        Advance();
        return expr;
    }
    else
    {
        // 这种情况是number
        auto factor = sema.SemaNumberExprNode(tok.value, tok.ty);
        Advance();
        return factor;
    }
};

bool Parser::Expect(TokenType tokenType)
{
    if (tok.tokenType == tokenType)
    {
        return true;
    }
    return false;
}

bool Parser::Consume(TokenType tokenType)
{
    if (tok.tokenType == tokenType)
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