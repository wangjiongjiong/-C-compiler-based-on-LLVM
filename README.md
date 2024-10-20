# C-compiler-based-on-LLVM
# 基于llvm构建c语言编译器

## 介绍
1. 使用C++语言实现，使用面向对象编程思想，有助于快速入门llvm这个庞大的体系
2. 后端基于llvm，可以快速了解llvm的IR使用，同时后续可以实现任何语言基于llvm的编译器

## C99语言
1. 实现了几乎C99语言的全部关键字
auto，short，int，long，float，double，char，struct，union，typedef，const，unsigned，signed，extern，register，static，volatile，void，if，else，switch，case，for，do，while，goto，continue，break，default，sizeof，return
只有一个enum关键字未实现，其中对于存储类型关键词 比如 static、register、extern，类型限定符 const、volatile，只是在解析类型声明符的时候为避免错误，能支持parser，并未支持对应的语义。
后续有机会会尝试实现

2. llvm ir 语言指令

- alloc
- load
- store
- getelementptr
- add/fadd
- sub/fsub
- mul
- div
- rem
- and
- or
- xor
- shl
- shr
- zext
- intcast
- not
- neg
- cmpeq
- cmpne
- cmpslt
- cmpsgt
- cmpsge
- br
- condbr
- phi

3. llvm ir 类型
- iN
- float
- double
- point
- array
- struct
- functiontype
