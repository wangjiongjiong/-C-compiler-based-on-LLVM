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

TO DO