#pragma once
#include "scope.h"
#include "ast.h"
#include "diag_engine.h"

// 语义分析模块

// parser模块需要语义分析
class Sema
{

private:
    DiagEngine &diagEngine;

public:
    Sema(DiagEngine &diagEngine) : diagEngine(diagEngine) {}
    // 变量声明的时候进行语义分析
    // 需要解析 变量name与变量类型
    std::shared_ptr<AstNode> SemaVariableDeclNode(Token tok, CType *ty);
    // 变量访问语义解析，只需要知道变量name即可
    std::shared_ptr<AstNode> SemaVariableAccessNode(Token tok);
    // 数字解析，获取数字的值以及类型
    std::shared_ptr<AstNode> SemaNumberExprNode(Token tok, CType *ty);
    // 赋值解析左右值传入
    std::shared_ptr<AstNode> SemaAssignExprNode(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right, Token tok);
    // 二元运算传入左右和操作数即可
    std::shared_ptr<AstNode> SemaBinaryExprNode(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right, OpCode op);

private:
    Scope scope;
};