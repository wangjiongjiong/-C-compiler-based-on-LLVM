#pragma once

#include <memory>
#include <vector>
#include "llvm/IR/Value.h"
#include "type.h"
#include "lexer.h"

// 节点头文件
// 组合者模式

/*
prog : stmt*
stmt : decl-stmt | expr-stmt | null-stmt | if-stmt
null-stmt : ";"
decl-stmt : "int" identifier ("," identifier (= expr)?)* ";"
if-stmt : "if" "(" expr ")" stmt ( "else" stmt )?
expr-stmt : expr ";"
expr : assign-expr | add-expr
assign-expr: identifier "=" expr
add-expr : mult-expr (("+" | "-") mult-expr)*
mult-expr : primary-expr (("*" | "/") primary-expr)*
primary-expr : identifier | number | "(" expr ")"
number: ([0-9])+
identifier : (a-zA-Z_)(a-zA-Z0-9_)*
*/

class Program;
class VariableDecl;
class AssignExpr;
class NumberExpr;
class VariableAccessExpr;
class BinaryExpr;
class IfStmt;
class DeclStmt;
class BlockStmt;

class Visitor
{
public:
    virtual ~Visitor() {};
    virtual llvm::Value *VisitProgram(Program *p) = 0;
    virtual llvm::Value *VisitIfStmt(IfStmt *p) = 0;
    virtual llvm::Value *VisitBlockStmt(BlockStmt *p) = 0;
    virtual llvm::Value *VisitDeclStmt(DeclStmt *p) = 0;
    virtual llvm::Value *VisitVariableDecl(VariableDecl *decl) = 0;
    virtual llvm::Value *VisitAssignExpr(AssignExpr *expr) = 0;
    virtual llvm::Value *VisitNumberExpr(NumberExpr *numberExpr) = 0;
    virtual llvm::Value *VisitBinaryExpr(BinaryExpr *binaryexpr) = 0;
    virtual llvm::Value *VisitVariableAccessExpr(VariableAccessExpr *variableAccessExpr) = 0;
};

// llvm rtti
// 方便指针做强制类型转换

class AstNode
{

public:
    enum Kind
    {
        ND_DeclStmt,
        ND_BlockStmt,
        ND_IfStmt,
        ND_VariableDecl,
        ND_BinaryExpr,
        ND_NumberFactor,
        ND_VariableAccessExpr,
        ND_AssignExpr

    };

private:
    const Kind kind;

public:
    virtual ~AstNode() {}
    CType *ty;
    Token tok;
    AstNode(Kind kind) : kind(kind) {}
    const Kind Getkind() const { return kind; }
    virtual llvm::Value *Accept(Visitor *v) { return nullptr; }
};

class BlockStmt : public AstNode
{
public:
    std::vector<std::shared_ptr<AstNode>> nodeVec;

public:
    BlockStmt() : AstNode(ND_BlockStmt) {}
    llvm::Value *Accept(Visitor *v) override
    {
        return v->VisitBlockStmt(this);
    }
    static bool classof(const AstNode *node)
    {
        return node->Getkind() == ND_DeclStmt;
    }
};

class DeclStmt : public AstNode
{
public:
    std::vector<std::shared_ptr<AstNode>> nodeVec;

public:
    DeclStmt() : AstNode(ND_DeclStmt) {}
    llvm::Value *Accept(Visitor *v) override
    {
        return v->VisitDeclStmt(this);
    }
    static bool classof(const AstNode *node)
    {
        return node->Getkind() == ND_DeclStmt;
    }
};

class IfStmt : public AstNode
{

public:
    std::shared_ptr<AstNode> condNode;
    std::shared_ptr<AstNode> thenNode;
    std::shared_ptr<AstNode> elseNode;

public:
    IfStmt() : AstNode(ND_IfStmt) {}
    llvm::Value *Accept(Visitor *v) override
    {
        return v->VisitIfStmt(this);
    }
    static bool classof(const AstNode *node)
    {
        return node->Getkind() == ND_IfStmt;
    }
};

class VariableDecl : public AstNode
{
public:
    VariableDecl() : AstNode(ND_VariableDecl) {}
    llvm::Value *Accept(Visitor *v) override
    {
        return v->VisitVariableDecl(this);
    }
    static bool classof(const AstNode *node)
    {
        return node->Getkind() == ND_VariableDecl;
    }
};

enum class OpCode
{
    add,
    sub,
    mul,
    div,
    equal_equal,
    not_equal,
    less,
    less_equal,
    greater,
    greater_equal

};

// 二元表达式
// 多了操作符左孩子其实就是左边的term
// 右孩子就是右边的term
class BinaryExpr : public AstNode
{
public:
    OpCode op;
    std::shared_ptr<AstNode> left;
    std::shared_ptr<AstNode> right;
    BinaryExpr() : AstNode(ND_BinaryExpr) {}
    llvm::Value *Accept(Visitor *v) override
    {
        return v->VisitBinaryExpr(this);
    }
    static bool classof(const AstNode *node)
    {
        return node->Getkind() == ND_BinaryExpr;
    }
};

// factor相当于就是终结符
class NumberExpr : public AstNode
{
public:
    NumberExpr() : AstNode(ND_NumberFactor) {}
    llvm::Value *Accept(Visitor *v) override
    {
        return v->VisitNumberExpr(this);
    }
    static bool classof(const AstNode *node)
    {
        return node->Getkind() == ND_NumberFactor;
    }
};

class VariableAccessExpr : public AstNode
{
public:
    VariableAccessExpr() : AstNode(ND_VariableAccessExpr) {}
    llvm::Value *Accept(Visitor *v) override
    {
        return v->VisitVariableAccessExpr(this);
    }
    static bool classof(const AstNode *node)
    {
        return node->Getkind() == ND_VariableAccessExpr;
    }
};

class AssignExpr : public AstNode
{
public:
    std::shared_ptr<AstNode> left;
    std::shared_ptr<AstNode> right;
    AssignExpr() : AstNode(ND_AssignExpr) {}
    llvm::Value *Accept(Visitor *v) override
    {
        return v->VisitAssignExpr(this);
    }
    static bool classof(const AstNode *node)
    {
        return node->Getkind() == ND_AssignExpr;
    }
};

// 相当于语法树的根节点也就是程序的入口
class Program
{
public:
    std::vector<std::shared_ptr<AstNode>> nodeVec;
};