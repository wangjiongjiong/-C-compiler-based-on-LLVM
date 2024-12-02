#pragma once
#include "lexer.h"
#include "ast.h"
#include "sema.h"

/*
prog : stmt*
stmt : decl-stmt | expr-stmt | null-stmt | if-stmt | block-stmt | for-stmt | break-stmt | continue-stmt
null-stmt : ";"
decl-stmt : "int" identifier ("," identifier (= expr)?)* ";"
if-stmt : "if" "(" expr ")" stmt ( "else" stmt )?
for-stmt : "for" "(" expr? ";" expr? ";" expr? ")" stmt
                     "for" "(" decl-stmt expr? ";" expr? ")" stmt
block-stmt: "{" stmt* "}"
break-stmt: "break" ";"
continue-stmt: "continue" ";"
expr-stmt : expr ";"
expr : assign-expr | logor-expr
assign-expr: identifier "=" expr
logor-expr: logand-expr ("||" logand-expr)*
logand-expr: bitor-expr ("&&" bitor-expr)*
bitor-expr: bitxor-expr ("|" bitxor-expr)*
bitxor-expr: bitand-expr ("^" bitand-expr)*
bitand-expr: equal-expr ("&" equal-expr)*
equal-expr : relational-expr (("==" | "!=") relational-expr)*
relational-expr: shift-expr (("<"|">"|"<="|">=") shift-expr)*
shift-expr: add-expr (("<<" | ">>") add-expr)*
add-expr : mult-expr (("+" | "-") mult-expr)*
mult-expr : primary-expr (("*" | "/" | "%") primary-expr)*
primary-expr : identifier | number | "(" expr ")"
number: ([0-9])+
identifier : (a-zA-Z_)(a-zA-Z0-9_)*
 */

// parser类
class Parser
{
private:
    // 要声明一个lex因为parser传入就是lex
    // 而lex其本质就是每次调用nexttoken用来解析token
    Lexer &lexer;
    Sema &sema;
    std::vector<std::shared_ptr<AstNode>> breakNodes;
    std::vector<std::shared_ptr<AstNode>> continueNodes;

public:
    Parser(Lexer &lexer, Sema &sema) : lexer(lexer), sema(sema)
    {
        // 最开始就要消费一个token
        Advance();
    }
    // 返回程序入口根节点
    std::shared_ptr<Program> ParseProgram();

private:
    std::shared_ptr<AstNode> ParseStmt(); // parser Stmt

    std::shared_ptr<AstNode> ParserDeclStmt();    // parser 声明语句 --- 经过修改的文法都是由语句构成的
    std::shared_ptr<AstNode> ParseIfStmt();       // parser If语句
    std::shared_ptr<AstNode> ParseBlockStmt();    // parser 块语句也就是{}
    std::shared_ptr<AstNode> ParseForStmt();      // parser for语句
    std::shared_ptr<AstNode> ParseBreakStmt();    // parser break语句
    std::shared_ptr<AstNode> ParseContinueStmt(); // parser continue语句
    std::shared_ptr<AstNode> ParseExprStmt();     // parser Expr语句

    std::shared_ptr<AstNode> ParseExpr(); // parser表达式

    std::shared_ptr<AstNode> ParseAssignExpr(); // parser 赋值表达式
    std::shared_ptr<AstNode> ParseLogOrExpr();  // parser 逻辑||表达式

    std::shared_ptr<AstNode> ParseLogAndExpr();     // parser 逻辑&&表达式
    std::shared_ptr<AstNode> ParseBitOrExpr();      // parser 位|表达式
    std::shared_ptr<AstNode> ParseBitXorExpr();     // parser 位^表达式
    std::shared_ptr<AstNode> ParseBitAndExpr();     // parser 位&表达式
    std::shared_ptr<AstNode> ParseEqualExpr();      // parser equal表达式
    std::shared_ptr<AstNode> ParseRelationalExpr(); // parser 关系表达式
    std::shared_ptr<AstNode> ParseShiftExpr();      // parser 移位表达式

    std::shared_ptr<AstNode> ParseAddExpr();   // parser 加号表达式
    std::shared_ptr<AstNode> ParseMultiExpr(); // parser 乘号表达式

    std::shared_ptr<AstNode> ParsePrimary(); // parser factor

    bool IsTypeName(); // 判断类型

    // 消费 token的函数
    // 检测当前token是否是该类型，不会消费
    bool Expect(TokenType tokenType);
    // 检测当前token，并且消费
    bool Consume(TokenType tokenType);
    // 前进一个token,一定会消费一个token
    void Advance();
    // 返回诊断引擎
    DiagEngine &GetDiagEngine() const
    {
        return lexer.GetDiagEngine();
    }

    Token tok;
};
