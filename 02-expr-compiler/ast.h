#pragma once

#include <memory>
#include <vector>
#include "llvm/IR/Value.h"

// 节点头文件
// 组合者模式

/*
prog : (expr? ";")*
expr : term (("+" | "-") trem)* ;
term : factor (("*" | "/") factor)* ;
factor : number | "(" expr ")" ;
number : ([0-9])+
*/

class Program;
class FactorExpr;
class BinaryExpr;
class Expr;

class Visitor
{
public:
    virtual ~Visitor() {};
    virtual llvm::Value *VisitProgram(Program *p) = 0;
    virtual llvm::Value *VisitExpr(Expr *expr) { return nullptr; };
    virtual llvm::Value *VisitBinaryExpr(BinaryExpr *binaryexpr) = 0;
    virtual llvm::Value *VisitFactorExpr(FactorExpr *factorexpr) = 0;
};

// 表达式
class Expr
{
public:
    virtual ~Expr() {};
    // 因为表达式本身不会被执行
    // 因此作为基类表示
    virtual llvm::Value *Accept(Visitor *v) { return nullptr; }
};

enum class OpCode
{
    add,
    sub,
    mul,
    div
};

// 二元表达式
// 多了操作符左孩子其实就是左边的term
// 右孩子就是右边的term
class BinaryExpr : public Expr
{
public:
    OpCode op;
    std::shared_ptr<Expr> left;
    std::shared_ptr<Expr> right;

    llvm::Value *Accept(Visitor *v) override
    {
        return v->VisitBinaryExpr(this);
    }
};

// factor相当于就是终结符
class FactorExpr : public Expr
{
public:
    int number;
    llvm::Value *Accept(Visitor *v) override
    {
        return v->VisitFactorExpr(this);
    }
};

// 相当于语法树的根节点也就是程序的入口
class Program
{
public:
    std::vector<std::shared_ptr<Expr>> expVec;
};