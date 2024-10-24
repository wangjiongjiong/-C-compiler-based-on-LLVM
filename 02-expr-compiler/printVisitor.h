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
    llvm::Value *VisitBinaryExpr(BinaryExpr *binaryexpr) override;
    llvm::Value *VisitFactorExpr(FactorExpr *factorexpr) override;
};