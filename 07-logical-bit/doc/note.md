# 无变量表达式的编译器

这个部分是一个不含变量的表达式的编译器
所谓不含变量其实就是一个直接由数字构成的表达式

## 构建语法
一般来讲设计编译器都是采用上下文无关语法的形式也就是ebnf
这里就直接给出了

prog : (expr? ";")* 
expr : term (("+" | "-") trem)* ;
term : factor (("*" | "/") factor)* ;
factor : number | "(" expr ")" ;
number : ([0-9])+ 

## Lexer 
词法分析，词法分析简单来说就是将char stream 转化成 token
lexer.h
lexer.cc

首先写一个 TokenType 用来标记 Token的类型

Token类用来表示token的属性

Lexer类中最关键的函数就是NextToken用来循环解析 char Stream

具体实现主要用到了llvm的api相对不是很复杂，直接看.h与.cc文件应该就可以明白

## parser

其实parser就是使用了一种访问者模式来遍历语法树，这里设计的很有考究
我们设计了一个visit的基本类
并且定义了四个虚函数用来实现多态
分别是progarm、expr、VisitBinary和factor
但是我们传入给parser的是program
具体实现可以看parser.h和parser.cc文件逻辑不是很难
其实本质来讲是根据语法构造来的

## codegen

因为我们从始至终的目的就是讲我们的源语言转换成llvm ir然后调用llvm
来进行后端编译，所以代码生成其实本质也是一种访问者模式只不过我们要把
语法树经过遍历生成llvm ir 这里面的难点和重点其实llvm ir的构造和设计
codegen.h和codegen.cc中都有详细注释说明