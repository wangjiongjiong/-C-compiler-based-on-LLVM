#include "sema.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"

// 声明局部变量语义分析
std::shared_ptr<AstNode> Sema::SemaVariableDeclNode(Token tok, CType *ty)
{
    // 1.检查是否出现重定义
    llvm::StringRef text(tok.ptr, tok.len);
    std::shared_ptr<Symbol> symbol = scope.FindVarSymbolInCurEnv(text);
    if (symbol)
    {
        // llvm::errs() << "Redefine varibale!!! " << text << "\n";
        diagEngine.Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::err_redefine, text);
    }
    // 2.无重定义，添加到符号表
    scope.AddSymbol(SymbolKind::LocalVariable, ty, text);

    auto Decl = std::make_shared<VariableDecl>();
    Decl->tok = tok;
    Decl->ty = ty;

    return Decl;
}

std::shared_ptr<AstNode> Sema::SemaVariableAccessNode(Token tok)
{

    // 1.检查是否出现未定义
    llvm::StringRef text(tok.ptr, tok.len);
    std::shared_ptr<Symbol> symbol = scope.FindVarSymbol(text);
    if (symbol == nullptr)
    {
        // llvm::errs() << "use undefined symbol: " << text << "\n";
        diagEngine.Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::err_undefine, text);
        // return nullptr; 内部会退出
    }

    auto expr = std::make_shared<VariableAccessExpr>();
    expr->tok = tok;
    // 变量访问不需要类型，类型在定义的时候声明了
    // expr->ty = ty;
    expr->ty = symbol->GetTy();
    return expr;
}

// 声明赋值表达式语义分析
std::shared_ptr<AstNode> Sema::SemaAssignExprNode(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right, Token tok)
{
    assert(left && right);
    /* if (!left || !right)
    {
        llvm::errs() << "left or right can't nullptr\n";
        return nullptr;
    } */
    // 赋值必须是左值
    if (!llvm::isa<VariableAccessExpr>(left.get()))
    {
        // llvm::errs() << "must be left value!\n";
        diagEngine.Report(llvm::SMLoc::getFromPointer(tok.ptr), diag::err_undefine);
        return nullptr;
    }

    auto assignExpr = std::make_shared<AssignExpr>();
    assignExpr->left = left;
    assignExpr->right = right;
    return assignExpr;
}

std::shared_ptr<AstNode> Sema::SemaBinaryExprNode(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right, OpCode op)
{

    auto binaryExpr = std::make_shared<BinaryExpr>();
    binaryExpr->op = op;
    binaryExpr->left = left;
    binaryExpr->right = right;

    return binaryExpr;
}

std::shared_ptr<AstNode> Sema::SemaNumberExprNode(Token tok, CType *ty)
{

    auto factor = std::make_shared<NumberExpr>();
    factor->tok = tok;
    factor->ty = ty;

    return factor;
}

std::shared_ptr<AstNode> Sema::SemaIfStmtNode(std::shared_ptr<AstNode> condNode, std::shared_ptr<AstNode> thenNode, std::shared_ptr<AstNode> elseNode)
{
    auto node = std::make_shared<IfStmt>();
    node->condNode = condNode;
    node->thenNode = thenNode;
    node->elseNode = elseNode;
    return node;
}

void Sema::EnterScope()
{
    scope.EnterScope();
}
void Sema::ExitScope()
{
    scope.ExitScope();
}