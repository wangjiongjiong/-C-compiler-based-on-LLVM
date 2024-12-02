#pragma once
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"
#include "type.h"
#include "diag_engine.h"

// char stream -> Token

enum class TokenType
{
    number,          // 数字
    identifier,      // 标识符
    kw_int,          // int
    kw_if,           // if
    kw_else,         // else
    kw_for,          // for
    kw_break,        // break
    kw_continue,     // continue
    minus,           // -
    plus,            // +
    star,            // *
    slash,           // "/"
    l_parent,        // "("
    r_parent,        // ")"
    semi,            // ";"
    equal,           // =
    comma,           // ,
    l_brace,         // {
    r_brace,         // }
    equal_equal,     // ==
    not_equal,       // !=
    less,            // <
    less_equal,      // <=
    greater,         // >
    greater_equal,   // >=
    pipe,            // |
    pipepipe,        // ||
    amp,             // &
    ampamp,          // &&
    percent,         // %
    less_less,       // <<
    greater_greater, // >>
    caret,           // ^
    eof              // end
};

class Token
{

public:
    TokenType tokenType;
    int row, col;
    // for number
    int value;

    // for debug && diag
    const char *ptr;
    int len;

    // for built-in type
    CType *ty;

    void Dump()
    {
        llvm::outs() << "{" << '"' << llvm::StringRef(ptr, len) << '"' << " row = " << row << ", col = " << col << "}\n";
    }

    // 返回一个有效的字符串
    static llvm::StringRef GetSpellingText(TokenType tokenType);
};

class Lexer
{

private:
    llvm::SourceMgr &mgr;
    DiagEngine &diagEngine;

public:
    Lexer(llvm::SourceMgr &mgr, DiagEngine &diagEngine) : mgr(mgr), diagEngine(diagEngine)
    {
        unsigned id = mgr.getMainFileID();
        llvm::StringRef buf = mgr.getMemoryBuffer(id)->getBuffer();
        BufPtr = buf.begin();
        LineHeadPtr = buf.begin();
        BufEnd = buf.end();
        row = 1;
    }

    void NextToken(Token &tok);

    void SaveState();

    void RestoreState();

    // 返回诊断引擎
    DiagEngine &GetDiagEngine() const
    {
        return diagEngine;
    }

private:
    bool StartWith(const char *p);

private:
    const char *BufPtr;
    const char *LineHeadPtr;
    const char *BufEnd;
    int row;

    struct State
    {
        const char *BufPtr;
        const char *LineHeadPtr;
        const char *BufEnd;
        int row;
    };

    State state;
};
