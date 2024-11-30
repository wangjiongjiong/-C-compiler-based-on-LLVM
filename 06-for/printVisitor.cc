#include "printVisitor.h"

PrintVisitor::PrintVisitor(std::shared_ptr<Program> program)
{
    VisitProgram(program.get());
}

llvm::Value *PrintVisitor::VisitProgram(Program *p)
{
    for (auto &node : p->nodeVec)
    {
        node->Accept(this);
        llvm::outs() << "\n";
    }
    return nullptr;
}

llvm::Value *PrintVisitor::VisitBlockStmt(BlockStmt *p)
{
    llvm::outs() << "\n{\n";
    for (const auto &stmt : p->nodeVec)
    {
        stmt->Accept(this);
        llvm::outs() << "\n";
    }
    llvm::outs() << "}";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitIfStmt(IfStmt *p)
{
    llvm::outs() << "if (";
    p->condNode->Accept(this);
    llvm::outs() << ")";
    p->thenNode->Accept(this);
    if (p->elseNode)
    {
        llvm::outs() << "\nelse";
        p->elseNode->Accept(this);
    }
    return nullptr;
}

llvm::Value *PrintVisitor::VisitForStmt(ForStmt *p)
{
    llvm::outs() << "for (";
    if (p->initNode)
    {
        p->initNode->Accept(this);
    }
    llvm::outs() << ";";
    if (p->condNode)
    {
        p->condNode->Accept(this);
    }
    llvm::outs() << ";";
    if (p->incNode)
    {
        p->incNode->Accept(this);
    }
    llvm::outs() << ")";
    if (p->bodyNode)
    {
        p->bodyNode->Accept(this);
    }
    return nullptr;
}

llvm::Value *PrintVisitor::VisitBreakStmt(BreakStmt *p)
{
    llvm::outs() << "break ;";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitContinueStmt(ContinueStmt *p)
{
    llvm::outs() << "continue ;";
    return nullptr;
}

llvm::Value *PrintVisitor::VisitDeclStmt(DeclStmt *p)
{
    for (auto &node : p->nodeVec)
    {
        node->Accept(this);
        // llvm::outs() << "\n";
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
    case OpCode::equal_equal:
        llvm::outs() << "==";
        break;
    case OpCode::not_equal:
        llvm::outs() << "!=";
        break;
    case OpCode::less:
        llvm::outs() << "<";
        break;
    case OpCode::less_equal:
        llvm::outs() << "<=";
        break;
    case OpCode::greater:
        llvm::outs() << ">";
        break;
    case OpCode::greater_equal:
        llvm::outs() << ">=";
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