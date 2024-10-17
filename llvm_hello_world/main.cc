#include <iostream>
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Verifier.h"
// llvm helloworld
// source -> llvm ir -> run -> output "hello,world"
// llvm ir 结尾.ll
// .ll -> module -> {glboal*,function*}
// function -> {basicBlock+} must have entryBlock

// gen -> module ->main -> puts("hello,world")

using namespace llvm;

// ModuleID = 'helloworld' s
// ource_filename = "helloworld"
//@gStr = private constant [12 x i8]
//@gStr = private constant [12 x i8] c"hello,world\00"
// declare i32 @main()
// declare i32 @puts(ptr)
// define i32 @main() {
// entry : % call = call i32 @puts(ptr @gStr)
//    ret i32 0
//}
int main()
{
    // c++ 输出
    // std::cout << "hello,world!" << std::endl;

    // ir 输出
    LLVMContext context;
    Module m("helloworld", context);
    IRBuilder<> irBuild(context);

    // int puts(char *)
    FunctionType *putsFuncType = FunctionType::get(irBuild.getInt32Ty(), {irBuild.getInt8PtrTy()}, false);
    Function *putsFunc = Function::Create(putsFuncType, GlobalValue::LinkageTypes::ExternalLinkage, "puts", m);

    llvm::Constant *c = ConstantDataArray::getString(context, "hello,world");
    GlobalVariable *gvar = new GlobalVariable(m, c->getType(), true, GlobalValue::LinkageTypes::PrivateLinkage, c, "gStr");

    FunctionType *mainFuncType = FunctionType::get(irBuild.getInt32Ty(), false);
    Function *mainFunc = Function::Create(mainFuncType, GlobalValue::LinkageTypes::ExternalLinkage, "main", m);
    BasicBlock *entryBB = BasicBlock::Create(context, "entry", mainFunc);
    irBuild.SetInsertPoint(entryBB);

    llvm::Value *gep = irBuild.CreateGEP(gvar->getType(), gvar, {irBuild.getInt64(0), irBuild.getInt64(0)});

    irBuild.CreateCall(putsFunc, {gep}, "call_puts");
    irBuild.CreateRet(irBuild.getInt32(0));

    verifyFunction(*putsFunc, &errs());
    verifyModule(m, &errs());

    m.print(outs(), nullptr);

    return 0;
}