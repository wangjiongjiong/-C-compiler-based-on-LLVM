#include "printVisitor.h"

PrintVisitor::PrintVisitor(std::shared_ptr<Program> program)
{
    VisitProgram(program.get());
}

void PrintVisitor::VisitProgram(Program *p)
{
    for (auto &expr : p->expVec)
    {
        expr->Accept(this);
        llvm::outs() << "\n";
    }
}
void PrintVisitor::VisitBinaryExpr(BinaryExpr *binaryexpr)
{
    binaryexpr->left->Accept(this);
    binaryexpr->right->Accept(this);
    switch (binaryexpr->op)
    {
    case OpCode::add:
        llvm::outs() << "+";
        break;
    case OpCode::sub:
        llvm::outs() << "-";
        break;
    case OpCode::mul:
        llvm::outs() << "*";
        break;
    case OpCode::div:
        llvm::outs() << "/";
        break;
    default:
        break;
    }
}
void PrintVisitor::VisitFactorExpr(FactorExpr *factorexpr)
{
    llvm::outs() << factorexpr->number;
}