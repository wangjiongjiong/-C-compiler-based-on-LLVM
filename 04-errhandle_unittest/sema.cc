#include "sema.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"

// 声明局部变量语义分析
std::shared_ptr<AstNode> Sema::SemaVariableDeclNode(llvm::StringRef name, CType *ty)
{
    // 1.检查是否出现重定义
    std::shared_ptr<Symbol> symbol = scope.FindVarSymbolInCurEnv(name);
    if (symbol)
    {
        llvm::errs() << "Redefine varibale!!! " << name << "\n";
        return nullptr;
    }
    // 2.无重定义，添加到符号表
    scope.AddSymbol(SymbolKind::LocalVariable, ty, name);

    auto variableDecl = std::make_shared<VariableDecl>();
    variableDecl->name = name;
    variableDecl->ty = ty;

    return variableDecl;
}

std::shared_ptr<AstNode> Sema::SemaVariableAccessNode(llvm::StringRef name)
{

    // 1.检查是否出现重定义
    std::shared_ptr<Symbol> symbol = scope.FindVarSymbol(name);
    if (symbol == nullptr)
    {
        llvm::errs() << "use undefined symbol: " << name << "\n";
        return nullptr;
    }

    auto expr = std::make_shared<VariableAccessExpr>();
    expr->name = name;
    // 变量访问不需要类型，类型在定义的时候声明了
    // expr->ty = ty;
    expr->ty = symbol->GetTy();
    return expr;
}

// 声明赋值表达式语义分析
std::shared_ptr<AstNode> Sema::SemaAssignExprNode(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right)
{
    if (!left || !right)
    {
        llvm::errs() << "left or right can't nullptr\n";
        return nullptr;
    }
    // 赋值必须是左值
    if (!llvm::isa<VariableAccessExpr>(left.get()))
    {
        llvm::errs() << "must be left value!\n";
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

std::shared_ptr<AstNode> Sema::SemaNumberExprNode(int number, CType *ty)
{

    auto factor = std::make_shared<NumberExpr>();
    factor->number = number;
    factor->ty = ty;

    return factor;
}