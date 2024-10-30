#include "lexer.h"

bool IsWhiteSpace(char ch)
{
    return ch == ' ' || ch == '\r' || ch == '\n';
}

bool IsDigit(char ch)
{
    return (ch >= '0' && ch <= '9');
}

bool IsLetter(char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '_');
}

Lexer::Lexer(llvm::StringRef sourceCode)
{
    LineHeadPtr = sourceCode.begin();
    BufPtr = sourceCode.begin();
    BufEnd = sourceCode.end();
    row = 1;
}

void Lexer::NextToken(Token &tok)
{
    // 行
    tok.row = row;

    // 1.删除空格
    while (IsWhiteSpace(*BufPtr))
    {
        if (*BufPtr == '\n')
        {
            row++;
            LineHeadPtr = BufPtr + 1;
        }
        BufPtr++;
    }
    // 列，先处理空格，处理空格完才是真正的token
    tok.col = BufPtr - LineHeadPtr + 1;

    // 2.判断是否结尾
    if (BufPtr >= BufEnd)
    {
        tok.tokenType = TokenType::eof;
        return;
    }
    const char *start = BufPtr;
    if (IsDigit(*BufPtr))
    {

        int len = 0;
        // 处理数字
        int number = 0;
        while (IsDigit(*BufPtr))
        {
            number = number * 10 + (*BufPtr++ - '0');
            len++;
        }
        tok.tokenType = TokenType::number;
        tok.value = number;
        tok.ty = CType::GetIntTy();
        tok.content = llvm::StringRef(start, len);
    }
    else if (IsLetter(*BufPtr))
    {
        while (IsLetter(*BufPtr) || IsDigit(*BufPtr))
        {
            BufPtr++;
        }
        tok.tokenType = TokenType::identifier;
        tok.content = llvm::StringRef(start, BufPtr - start);
        if (tok.content == "int")
        {
            tok.tokenType = TokenType::kw_int;
        }
    }
    else
    {
        switch (*BufPtr)
        {
        case '+':
        {
            tok.tokenType = TokenType::plus;
            BufPtr++;
            tok.content = llvm::StringRef(start, 1);
            break;
        }
        case '-':
        {
            tok.tokenType = TokenType::minus;
            BufPtr++;
            tok.content = llvm::StringRef(start, 1);
            break;
        }
        case '*':
        {
            tok.tokenType = TokenType::star;
            BufPtr++;
            tok.content = llvm::StringRef(start, 1);
            break;
        }
        case '/':
        {
            tok.tokenType = TokenType::slash;
            BufPtr++;
            tok.content = llvm::StringRef(start, 1);
            break;
        }
        case ';':
        {
            tok.tokenType = TokenType::semi;
            BufPtr++;
            tok.content = llvm::StringRef(start, 1);
            break;
        }
        case '(':
        {
            tok.tokenType = TokenType::l_parent;
            BufPtr++;
            tok.content = llvm::StringRef(start, 1);
            break;
        }
        case ')':
        {
            tok.tokenType = TokenType::r_parent;
            BufPtr++;
            tok.content = llvm::StringRef(start, 1);
            break;
        }
        case '=':
        {
            tok.tokenType = TokenType::equal;
            BufPtr++;
            tok.content = llvm::StringRef(start, 1);
            break;
        }
        case ',':
        {
            tok.tokenType = TokenType::comma;
            BufPtr++;
            tok.content = llvm::StringRef(start, 1);
            break;
        }
        default:
        {
            tok.tokenType = TokenType::unknown;
            break;
            tok.content = llvm::StringRef(start, 1);
        }
        }
    }
}