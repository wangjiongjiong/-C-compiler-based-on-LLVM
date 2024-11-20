#include "codegen.h"
#include "llvm/IR/Verifier.h"
using namespace llvm;

// llvm  ir 常量折叠
/*
; ModuleID = 'expr'
source_filename = "expr"

@0 = private unnamed_addr constant [13 x i8] c"expr val:%d\0A\00", align 1
@1 = private unnamed_addr constant [13 x i8] c"expr val:%d\0A\00", align 1
@2 = private unnamed_addr constant [13 x i8] c"expr val:%d\0A\00", align 1

declare i32 @print(ptr, ...)

define i32 @main() {
entry:
  %0 = call i32 (ptr, ...) @print(ptr @0, i32 4)
  %1 = call i32 (ptr, ...) @print(ptr @1, i32 9)
  %2 = call i32 (ptr, ...) @print(ptr @2, i32 16)
  ret i32 0
} */

// llvm ir的基本结构
// 是由function和全局变量构成
// 这里面就只有函数了因为是表达式语言不支持变量
// 一个简单的流程就是
// 先要获取functionType
// 然后创建funtion使用create函数
// llvm ir中的函数是有需多block构成
// 这里需要一个BasicBlock
// IR Builder 在 module, function, basic block 中跟踪下一行指令的插入位置 （ 插入点 ）
// 简单理解就是ir指令的插入
// 重点还是要理解llvm ir 是如何工作的
llvm::Value *CodeGen::VisitProgram(Program *p)
{
    // printf
    auto printFunction = FunctionType::get(irBuilder.getInt32Ty(), {irBuilder.getInt8PtrTy()}, true);
    auto printFunc = Function::Create(printFunction, GlobalValue::ExternalLinkage, "printf", module.get());

    // main
    // 所有表达式放到main函数中

    auto mFunctionType = FunctionType::get(irBuilder.getInt32Ty(), false);
    auto mFunc = Function::Create(mFunctionType, GlobalValue::ExternalLinkage, "main", module.get());
    BasicBlock *entryBB = BasicBlock::Create(context, "entry", mFunc);
    irBuilder.SetInsertPoint(entryBB);
    // 记录一下当前函数
    curFunc = mFunc;

    llvm::Value *lastVal = nullptr;
    for (auto &node : p->nodeVec)
    {
        lastVal = node->Accept(this);
    }
    if (lastVal)
    {
        irBuilder.CreateCall(printFunc, {irBuilder.CreateGlobalStringPtr("expr val:%d\n"), lastVal});
    }
    else
    {
        irBuilder.CreateCall(printFunc, {irBuilder.CreateGlobalStringPtr("last inst is not expr val!!!")});
    }

    // 返回指令
    llvm::Value *ret = irBuilder.CreateRet(irBuilder.getInt32(0));
    verifyFunction(*mFunc);

    module->print(llvm::outs(), nullptr);
    return ret;
}
llvm::Value *CodeGen::VisitBinaryExpr(BinaryExpr *binaryexpr)
{
    llvm::Value *left = binaryexpr->left->Accept(this);
    llvm::Value *right = binaryexpr->right->Accept(this);

    switch (binaryexpr->op)
    {
    case OpCode::add:
        // llvm::outs() << "+";
        return irBuilder.CreateNSWAdd(left, right);
        break;
    case OpCode::sub:
        // llvm::outs() << "-";
        return irBuilder.CreateNSWSub(left, right);
        break;
    case OpCode::mul:
        // llvm::outs() << "*";
        return irBuilder.CreateNSWMul(left, right);
        break;
    case OpCode::div:
        // llvm::outs() << "/";
        return irBuilder.CreateSDiv(left, right);
        break;
    default:
        break;
    }

    return nullptr;
}
llvm::Value *CodeGen::VisitNumberExpr(NumberExpr *numberExpr)
{
    return irBuilder.getInt32(numberExpr->tok.value);
}
llvm::Value *CodeGen::VisitDeclStmt(DeclStmt *p)
{
    llvm::Value *lastVal = nullptr;
    for (const auto &node : p->nodeVec)
    {
        lastVal = node->Accept(this);
    }
    return lastVal;
}

llvm::Value *CodeGen::VisitBlockStmt(BlockStmt *p)
{
    llvm::Value *lastVal = nullptr;
    for (const auto &stmt : p->nodeVec)
    {
        lastVal = stmt->Accept(this);
    }

    return lastVal;
}

// 划分基本块
llvm::Value *CodeGen::VisitIfStmt(IfStmt *p)
{
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(context, "cond", curFunc);
    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(context, "then", curFunc);
    llvm::BasicBlock *elseBB = nullptr;
    if (p->elseNode)
    {
        elseBB = llvm::BasicBlock::Create(context, "else", curFunc);
    }
    llvm::BasicBlock *lastBB = llvm::BasicBlock::Create(context, "last", curFunc);

    // 无条件跳转，llvm不会自动跳转的
    irBuilder.CreateBr(condBB);

    irBuilder.SetInsertPoint(condBB);
    llvm::Value *val = p->condNode->Accept(this);
    /// 整型比较指令
    llvm::Value *condVal = irBuilder.CreateICmpNE(val, irBuilder.getInt32(0));
    if (p->elseNode)
    {
        // else存在的情况下
        irBuilder.CreateCondBr(condVal, thenBB, elseBB);

        // 创建thenbb
        irBuilder.SetInsertPoint(thenBB);
        p->thenNode->Accept(this);
        irBuilder.CreateBr(lastBB);
        // 创建else
        irBuilder.SetInsertPoint(elseBB);
        p->elseNode->Accept(this);
        irBuilder.CreateBr(lastBB);
    }
    // else不存在的情况
    else
    {
        irBuilder.CreateCondBr(condVal, thenBB, lastBB);

        // 创建thenbb
        irBuilder.SetInsertPoint(thenBB);
        p->thenNode->Accept(this);
        irBuilder.CreateBr(lastBB);
    }

    irBuilder.SetInsertPoint(lastBB);

    return nullptr;
}

llvm::Value *CodeGen::VisitVariableDecl(VariableDecl *decl)
{

    // 变量声明要申请地址
    llvm::Type *ty = nullptr;
    if (decl->ty = CType::GetIntTy())
    {
        ty = irBuilder.getInt32Ty();
    }
    llvm::StringRef text(decl->tok.ptr, decl->tok.len);
    // 创建地址
    llvm::Value *value = irBuilder.CreateAlloca(ty, nullptr, text);
    varAddrTypeMap.insert({text, {value, ty}});
    return value;
}

// a = 3  => rvalue;
llvm::Value *CodeGen::VisitAssignExpr(AssignExpr *expr)
{
    auto left = expr->left;
    // 这里得进行类型转换不然无法获取到name
    VariableAccessExpr *varAccessExpr = (VariableAccessExpr *)left.get();
    llvm::StringRef text(varAccessExpr->tok.ptr, varAccessExpr->tok.len);
    std::pair pair = varAddrTypeMap[text];
    llvm::Value *leftvarAddr = pair.first;
    llvm::Type *ty = pair.second;
    llvm::Value *rightvalue = expr->right->Accept(this);
    // irBuilder.CreateStore 是 LLVM 中的一个函数，用于在 LLVM IR 中创建一个存储（store）指令。
    // 这个指令将一个值存储到一个内存位置。
    irBuilder.CreateStore(rightvalue, leftvarAddr);
    return irBuilder.CreateLoad(ty, leftvarAddr, text);
}
llvm::Value *CodeGen::VisitVariableAccessExpr(VariableAccessExpr *variableAccessExpr)
{
    llvm::StringRef text(variableAccessExpr->tok.ptr, variableAccessExpr->tok.len);
    std::pair pair = varAddrTypeMap[text];
    llvm::Value *varAddr = pair.first;
    llvm::Type *ty = pair.second;
    // LLVM 中用于创建加载（load）指令的函数调用。
    // 这个指令从指定的内存地址读取数据，并返回一个值
    return irBuilder.CreateLoad(ty, varAddr, text);
}