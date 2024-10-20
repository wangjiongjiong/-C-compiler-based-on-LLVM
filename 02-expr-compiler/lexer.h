#pragma once
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"

// char stream -> Token

enum class TokenType
{
    unknown,  // 未知类型
    number,   // 数字
    minus,    // -
    plus,     // +
    star,     // *
    slash,    // "/"
    l_parent, // "("
    r_parent, // ")"
    seml,     // ";"
    eof       // end
};

class Token
{

public:
    int row, col;
    TokenType tokenType;
    int value;
    llvm::StringRef content;
    Token()
    {
        row = col = -1;
        tokenType = TokenType::unknown;
        value = -1;
    }

    void Dump()
    {
        llvm::outs() << "{" << content << "row = " << row << ", col = " << col << "}\n";
    }
};

class Lexer
{

public:
    Lexer(llvm::StringRef sourceCode);

    void NextToken(Token &tok);

private:
    const char *BufPtr;
    const char *LineHeadPtr;
    const char *BufEnd;
    int row;
};
