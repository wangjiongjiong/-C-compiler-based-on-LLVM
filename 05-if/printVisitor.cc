#include "printVisitor.h"

PrintVisitor::PrintVisitor(std::shared_ptr<Program> program)
{
    VisitProgram(program.get());
}

llvm::Value *PrintVisitor::VisitProgram(Program *p)
{
    for (auto &expr : p->expVec)
    {
        expr->Accept(this);
        llvm::outs() << "\n";
    }
    return nullptr;
}

llvm::Value *PrintVisitor::VisitVariableDecl(VariableDecl *decl)
{
    if (decl->ty == CType::GetIntTy())
    {
        llvm::StringRef text(decl->tok.ptr, decl->tok.len);
        llvm::outs() << "int " << text << ";";
    }
    return nullptr;
}

llvm::Value *PrintVisitor::VisitAssignExpr(AssignExpr *expr)
{
    expr->left->Accept(this);
    llvm::outs() << " = ";
    expr->right->Accept(this);
    return nullptr;
}

llvm::Value *PrintVisitor::VisitBinaryExpr(BinaryExpr *binaryexpr)
{
    binaryexpr->left->Accept(this);
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
    binaryexpr->right->Accept(this);
    return nullptr;
}

llvm::Value *PrintVisitor::VisitNumberExpr(NumberExpr *numberExpr)
{
    llvm::outs() << llvm::StringRef(numberExpr->tok.ptr, numberExpr->tok.len);
    return nullptr;
}

llvm::Value *PrintVisitor::VisitVariableAccessExpr(VariableAccessExpr *variableAccessExpr)
{
    llvm::outs() << llvm::StringRef(variableAccessExpr->tok.ptr, variableAccessExpr->tok.len);
    return nullptr;
}