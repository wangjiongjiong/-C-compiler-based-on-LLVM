#include "scope.h"

Scope::Scope()
{
    // 主环境
    envs.push_back(std::make_shared<Env>());
}
void Scope::EnterScope()
{
    envs.push_back(std::make_shared<Env>());
}
void Scope::ExitScope()
{
    envs.pop_back();
}
std::shared_ptr<Symbol> Scope::FindVarSymbol(llvm::StringRef name)
{
    // 在环境中倒叙查找
    for (auto it = envs.rbegin(); it != envs.rend(); ++it)
    {
        // 找到了symbol直接返回
        auto &table = (*it)->variableSymbolTable;
        if (table.count(name) > 0)
        {
            return table[name];
        }
    }
    return nullptr;
}
std::shared_ptr<Symbol> Scope::FindVarSymbolInCurEnv(llvm::StringRef name)
{
    // 当前环境查找，envs最后放进去的就是当前的环境
    auto &table = (envs.back())->variableSymbolTable;
    if (table.count(name) > 0)
    {
        return table[name];
    }
    return nullptr;
}
void Scope::AddSymbol(SymbolKind kind, CType *ty, llvm::StringRef name)
{
    // 构建一个symbol
    auto symbol = std::make_shared<Symbol>(kind, ty, name);
    // 直接在环境中插入即可
    envs.back()->variableSymbolTable.insert({name, symbol});
}