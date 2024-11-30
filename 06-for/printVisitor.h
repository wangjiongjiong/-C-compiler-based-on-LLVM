#pragma once
#include "ast.h"
#include "parser.h"

// 访问者模式
// 就是遍历语法树
class PrintVisitor : public Visitor
{
public:
    PrintVisitor(std::shared_ptr<Program> program);

    llvm::Value *VisitProgram(Program *p) override;
    llvm::Value *VisitBlockStmt(BlockStmt *p) override;
    llvm::Value *VisitIfStmt(IfStmt *p) override;
    llvm::Value *VisitForStmt(ForStmt *p) override;
    llvm::Value *VisitBreakStmt(BreakStmt *p) override;
    llvm::Value *VisitContinueStmt(ContinueStmt *p) override;
    llvm::Value *VisitDeclStmt(DeclStmt *p) override;
    llvm::Value *VisitVariableDecl(VariableDecl *decl) override;
    llvm::Value *VisitAssignExpr(AssignExpr *expr) override;
    llvm::Value *VisitNumberExpr(NumberExpr *numberExpr) override;
    llvm::Value *VisitBinaryExpr(BinaryExpr *binaryexpr) override;
    llvm::Value *VisitVariableAccessExpr(VariableAccessExpr *variableAccessExpr) override;
};