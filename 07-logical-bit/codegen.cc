#include "codegen.h"
#include "llvm/IR/Verifier.h"
using namespace llvm;

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
    /* llvm::Value *left = binaryexpr->left->Accept(this);
    llvm::Value *right = binaryexpr->right->Accept(this);
 */
    switch (binaryexpr->op)
    {
    case OpCode::add:
    {
        llvm::Value *left = binaryexpr->left->Accept(this);
        llvm::Value *right = binaryexpr->right->Accept(this);
        return irBuilder.CreateNSWAdd(left, right);
        break;
    }

    case OpCode::sub:
    {
        llvm::Value *left = binaryexpr->left->Accept(this);
        llvm::Value *right = binaryexpr->right->Accept(this);
        return irBuilder.CreateNSWSub(left, right);
        break;
    }
    case OpCode::mul:
    {
        llvm::Value *left = binaryexpr->left->Accept(this);
        llvm::Value *right = binaryexpr->right->Accept(this);
        return irBuilder.CreateNSWMul(left, right);
        break;
    }

    case OpCode::div:
    {
        llvm::Value *left = binaryexpr->left->Accept(this);
        llvm::Value *right = binaryexpr->right->Accept(this);
        return irBuilder.CreateSDiv(left, right);
        break;
    }

    case OpCode::mod:
    {
        llvm::Value *left = binaryexpr->left->Accept(this);
        llvm::Value *right = binaryexpr->right->Accept(this);
        return irBuilder.CreateSRem(left, right);
        break;
    }

    case OpCode::bitAnd:
    {
        llvm::Value *left = binaryexpr->left->Accept(this);
        llvm::Value *right = binaryexpr->right->Accept(this);
        return irBuilder.CreateAnd(left, right);
        break;
    }

    case OpCode::bitOr:
    {
        llvm::Value *left = binaryexpr->left->Accept(this);
        llvm::Value *right = binaryexpr->right->Accept(this);
        return irBuilder.CreateOr(left, right);
        break;
    }

    case OpCode::bitXor:
    {
        llvm::Value *left = binaryexpr->left->Accept(this);
        llvm::Value *right = binaryexpr->right->Accept(this);
        return irBuilder.CreateXor(left, right);
        break;
    }

    case OpCode::leftShift:
    {
        llvm::Value *left = binaryexpr->left->Accept(this);
        llvm::Value *right = binaryexpr->right->Accept(this);
        return irBuilder.CreateShl(left, right);
        break;
    }

    case OpCode::rightShift:
    {
        llvm::Value *left = binaryexpr->left->Accept(this);
        llvm::Value *right = binaryexpr->right->Accept(this);
        return irBuilder.CreateAShr(left, right);
        break;
    }

    case OpCode::logAnd:
    {
        /// A&&B

        llvm::BasicBlock *nextBB = llvm::BasicBlock::Create(context, "nextBB", curFunc);
        llvm::BasicBlock *falseBB = llvm::BasicBlock::Create(context, "falseBB", curFunc);
        llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(context, "mergeBB", curFunc);
        // A的值
        llvm::Value *left = binaryexpr->left->Accept(this);
        /// 整型比较指令
        llvm::Value *val = irBuilder.CreateICmpNE(left, irBuilder.getInt32(0));
        irBuilder.CreateCondBr(val, nextBB, falseBB);

        irBuilder.SetInsertPoint(nextBB);

        // B的值
        llvm::Value *right = binaryexpr->right->Accept(this);
        /// 整型比较指令
        right = irBuilder.CreateICmpNE(right, irBuilder.getInt32(0));
        /// 扩展到32位的0或1
        right = irBuilder.CreateZExt(right, irBuilder.getInt32Ty());
        irBuilder.CreateBr(mergeBB);

        /// right 这个值所在的基本块并不一定是之前的nextBB
        /// 原因就是 binaryexpr->right->Accept(this);内部会生成新的指令块

        /// 拿到当前插入的块,建立一个新的值和基本块 {right,nextBB}
        nextBB = irBuilder.GetInsertBlock();

        falseBB->insertInto(curFunc);
        irBuilder.SetInsertPoint(falseBB);
        irBuilder.CreateBr(mergeBB);

        mergeBB->insertInto(curFunc);
        irBuilder.SetInsertPoint(mergeBB);
        llvm::PHINode *phi = irBuilder.CreatePHI(irBuilder.getInt32Ty(), 2);
        phi->addIncoming(right, nextBB);
        phi->addIncoming(irBuilder.getInt32(0), falseBB);

        return phi;
    }
    case OpCode::logOr:
    {
        /// A || B && C

        llvm::BasicBlock *nextBB = llvm::BasicBlock::Create(context, "nextBB", curFunc);
        llvm::BasicBlock *trueBB = llvm::BasicBlock::Create(context, "trueBB", curFunc);
        llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(context, "mergeBB", curFunc);
        // A的值
        llvm::Value *left = binaryexpr->left->Accept(this);
        /// 整型比较指令
        llvm::Value *val = irBuilder.CreateICmpNE(left, irBuilder.getInt32(0));
        irBuilder.CreateCondBr(val, trueBB, nextBB);

        irBuilder.SetInsertPoint(nextBB);
        // 右子树内部也是生成了基本块

        // B的值
        llvm::Value *right = binaryexpr->right->Accept(this);
        /// 整型比较指令
        right = irBuilder.CreateICmpNE(right, irBuilder.getInt32(0));
        /// 扩展到32位的0或1
        right = irBuilder.CreateZExt(right, irBuilder.getInt32Ty());
        irBuilder.CreateBr(mergeBB);
        /// right 这个值所在的基本块并不一定是之前的nextBB
        /// 原因就是 binaryexpr->right->Accept(this);内部会生成新的指令块

        /// 拿到当前插入的块,建立一个新的值和基本块 {right,nextBB}
        nextBB = irBuilder.GetInsertBlock();

        trueBB->insertInto(curFunc);
        irBuilder.SetInsertPoint(trueBB);
        irBuilder.CreateBr(mergeBB);

        mergeBB->insertInto(curFunc);
        irBuilder.SetInsertPoint(mergeBB);
        llvm::PHINode *phi = irBuilder.CreatePHI(irBuilder.getInt32Ty(), 2);
        phi->addIncoming(right, nextBB);
        phi->addIncoming(irBuilder.getInt32(1), trueBB);

        return phi;
    }
    case OpCode::equal_equal:
    {
        llvm::Value *left = binaryexpr->left->Accept(this);
        llvm::Value *right = binaryexpr->right->Accept(this);
        /// 返回的是一个字节的类型
        llvm::Value *val = irBuilder.CreateICmpEQ(left, right);
        return irBuilder.CreateIntCast(val, irBuilder.getInt32Ty(), true);
        break;
    }
    case OpCode::not_equal:
    {
        llvm::Value *left = binaryexpr->left->Accept(this);
        llvm::Value *right = binaryexpr->right->Accept(this);
        llvm::Value *val = irBuilder.CreateICmpNE(left, right);
        return irBuilder.CreateIntCast(val, irBuilder.getInt32Ty(), true);
        break;
    }
    case OpCode::less:
    {
        llvm::Value *left = binaryexpr->left->Accept(this);
        llvm::Value *right = binaryexpr->right->Accept(this);
        llvm::Value *val = irBuilder.CreateICmpSLT(left, right);
        return irBuilder.CreateIntCast(val, irBuilder.getInt32Ty(), true);
        break;
    }
    case OpCode::less_equal:
    {
        llvm::Value *left = binaryexpr->left->Accept(this);
        llvm::Value *right = binaryexpr->right->Accept(this);
        llvm::Value *val = irBuilder.CreateICmpSLE(left, right);
        return irBuilder.CreateIntCast(val, irBuilder.getInt32Ty(), true);
        break;
    }
    case OpCode::greater:
    {
        llvm::Value *left = binaryexpr->left->Accept(this);
        llvm::Value *right = binaryexpr->right->Accept(this);
        llvm::Value *val = irBuilder.CreateICmpSGT(left, right);
        return irBuilder.CreateIntCast(val, irBuilder.getInt32Ty(), true);
        break;
    }
    case OpCode::greater_equal:
    {
        llvm::Value *left = binaryexpr->left->Accept(this);
        llvm::Value *right = binaryexpr->right->Accept(this);
        llvm::Value *val = irBuilder.CreateICmpSGE(left, right);
        return irBuilder.CreateIntCast(val, irBuilder.getInt32Ty(), true);
        break;
    }
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

llvm::Value *CodeGen::VisitForStmt(ForStmt *p)
{
    // 定义五个基本块
    // 初始化基本块
    llvm::BasicBlock *initBB = llvm::BasicBlock::Create(context, "for,init", curFunc);
    // 条件基本块
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(context, "for,cond", curFunc);
    // inc基本块
    llvm::BasicBlock *incBB = llvm::BasicBlock::Create(context, "for,inc", curFunc);
    // body基本快
    llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(context, "for,body", curFunc);
    // last基本块
    llvm::BasicBlock *lastBB = llvm::BasicBlock::Create(context, "for,last", curFunc);

    breakBBs.insert({p, lastBB});
    continueBBs.insert({p, incBB});

    // init 块
    irBuilder.CreateBr(initBB);
    irBuilder.SetInsertPoint(initBB);
    if (p->initNode)
    {
        p->initNode->Accept(this);
    }
    // 构建条件块
    irBuilder.CreateBr(condBB);
    irBuilder.SetInsertPoint(condBB);
    if (p->condNode)
    {
        llvm::Value *val = p->condNode->Accept(this);
        // 返回的是一个字节的值
        llvm::Value *condval = irBuilder.CreateICmpNE(val, irBuilder.getInt32(0));
        // 条件成功跳转到bodybb
        irBuilder.CreateCondBr(condval, bodyBB, lastBB);
    }
    else
    {
        // 无条件得话直接跳转
        irBuilder.CreateBr(bodyBB);
    }
    // 插入body块
    irBuilder.SetInsertPoint(bodyBB);
    if (p->bodyNode)
    {
        p->bodyNode->Accept(this);
    }
    // 循环条件块
    irBuilder.CreateBr(incBB);
    irBuilder.SetInsertPoint(incBB);
    if (p->incNode)
    {
        p->incNode->Accept(this);
    }
    irBuilder.CreateBr(condBB);

    breakBBs.erase(p);
    continueBBs.erase(p);
    irBuilder.SetInsertPoint(lastBB);

    return nullptr;
}

llvm::Value *CodeGen::VisitBreakStmt(BreakStmt *p)
{
    /// 生成一个跳转指令，跳转到last中
    // jump lastBB
    llvm::BasicBlock *bb = breakBBs[p->target.get()];
    irBuilder.CreateBr(bb);

    // 无效的块指令
    llvm::BasicBlock *out = llvm::BasicBlock::Create(context, "for.break.death", curFunc);
    irBuilder.SetInsertPoint(out);

    return nullptr;
}

llvm::Value *CodeGen::VisitContinueStmt(ContinueStmt *p)
{
    /// 生成一个跳转指令，跳转到inc中
    // jump incBB
    llvm::BasicBlock *bb = continueBBs[p->target.get()];
    irBuilder.CreateBr(bb);

    // 无效的块指令
    llvm::BasicBlock *out = llvm::BasicBlock::Create(context, "for.continue.death", curFunc);
    irBuilder.SetInsertPoint(out);

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