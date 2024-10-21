#pragma once
#include "ast.h"
#include "parser.h"
class PrintVisitor : public Visitor
{
public:
    PrintVisitor(std::shared_ptr<Program> program);

    void VisitProgram(Program *p) override;
    void VisitBinaryExpr(BinaryExpr *binaryexpr) override;
    void VisitFactorExpr(FactorExpr *factorexpr) override;
};