#pragma once
#include "llvm/ADT/StringMap.h"
#include "type.h"
#include <vector>
#include <memory>

// 枚举类来存储各种类型
// 当前只有局部变量后续可以添加很多
// 全局变量，结构体，枚举...
enum class SymbolKind

{
    LocalVariable
};

// 描述符号信息的类
class Symbol
{
private:
    SymbolKind kind;
    CType *ty;
    llvm::StringRef name;

public:
    Symbol(SymbolKind kind, CType *ty, llvm::StringRef name) : kind(kind), ty(ty), name(name) {}
    CType *GetTy() { return ty; }
};

class Env
{
public:
    // 存储符号信息
    llvm::StringMap<std::shared_ptr<Symbol>> variableSymbolTable;
};

class Scope
{

private:
    // 存储的环境信息，也就是一个{}就是一个作用域
    std::vector<std::shared_ptr<Env>> envs;

public:
    Scope();
    void EnterScope();
    void ExitScope();
    std::shared_ptr<Symbol> FindVarSymbol(llvm::StringRef name);
    std::shared_ptr<Symbol> FindVarSymbolInCurEnv(llvm::StringRef name);
    void AddSymbol(SymbolKind kind, CType *ty, llvm::StringRef name);
};