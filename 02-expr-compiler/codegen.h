#pragma once

#include "ast.h"
#include "parser.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"

// 代码生成模块
// 其实代码生成也是一个访问者模式
// 相当于遍历语法树只不过需要用llvm ir的模式进行输出
class CodeGen : public Visitor
{
public:
    // 构造函数
    // 建立一个module指针
    // 需要对visit里面的虚函数进行重写，并带上llvm::Value返回值
    CodeGen(std::shared_ptr<Program> program)
    {
        module = std::make_shared<llvm::Module>("expr", context);
        VisitProgram(program.get());
    }

private:
    llvm::Value *VisitProgram(Program *p) override;
    llvm::Value *VisitBinaryExpr(BinaryExpr *binaryexpr) override;
    llvm::Value *VisitFactorExpr(FactorExpr *factorexpr) override;

private:
    // 这部分内容是优先于构造函数的生成
    llvm::LLVMContext context;
    llvm::IRBuilder<> irBuilder{context};
    std::shared_ptr<llvm::Module> module;
};