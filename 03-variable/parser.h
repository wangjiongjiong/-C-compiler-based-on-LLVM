#pragma once
#include "lexer.h"
#include "ast.h"
#include "sema.h"

// parser类
class Parser
{
private:
    // 要声明一个lex因为parser传入就是lex
    // 而lex其本质就是每次调用nexttoken用来解析token
    Lexer &lexer;
    Sema &sema;

public:
    Parser(Lexer &lexer, Sema &sema) : lexer(lexer), sema(sema)
    {
        // 最开始就要消费一个token
        Advance();
    }
    // 返回程序入口根节点
    std::shared_ptr<Program> ParseProgram();

private:
    // parser表达式
    std::shared_ptr<AstNode> ParseExpr();
    // parser term
    std::shared_ptr<AstNode> ParseTerm();
    // parser factor
    std::shared_ptr<AstNode> ParseFactor();
    // parser 声明
    std::vector<std::shared_ptr<AstNode>> ParserDecl();

    // 消费 token的函数
    // 检测当前token是否是该类型，不会消费
    bool Expect(TokenType tokenType);
    // 检测当前token，并且消费
    bool Consume(TokenType tokenType);
    // 前进一个token,一定会消费一个token
    void Advance();

    Token tok;
};
