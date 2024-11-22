#pragma once
#include "ast.h"
#include "parser.h"

// 访问者模式
// 就是遍历语法树
class PrintVisitor : public Visitor
{
public:
    PrintVisitor(std::shared_ptr<Program> program);

    /*
    virtual llvm::Value *VisitProgram(Program *p) = 0;
    virtual llvm::Value *VisitVariableDeclExpr(VariableDecl *decl) = 0;
    virtual llvm::Value *VisitAssignExpr(AssignExpr *expr) = 0;
    virtual llvm::Value *VisitNumberExpr(NumberExpr *expr) = 0;
    virtual llvm::Value *VisitBinaryExpr(BinaryExpr *binaryexpr) = 0;
    virtual llvm::Value *VisitVariableAccessExpr(VariableAccessExpr *factorexpr) = 0; */

    llvm::Value *VisitProgram(Program *p) override;
    llvm::Value *VisitBlockStmt(BlockStmt *p) override;
    llvm::Value *VisitIfStmt(IfStmt *p) override;
    llvm::Value *VisitDeclStmt(DeclStmt *p) override;
    llvm::Value *VisitVariableDecl(VariableDecl *decl) override;
    llvm::Value *VisitAssignExpr(AssignExpr *expr) override;
    llvm::Value *VisitNumberExpr(NumberExpr *numberExpr) override;
    llvm::Value *VisitBinaryExpr(BinaryExpr *binaryexpr) override;
    llvm::Value *VisitVariableAccessExpr(VariableAccessExpr *variableAccessExpr) override;
};