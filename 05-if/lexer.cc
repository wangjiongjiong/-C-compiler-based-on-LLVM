#include "lexer.h"

/* number,         // 数字
    identifier, // 标识符
    kw_int,     // int
    minus,      // -
    plus,       // +
    star,       // *
    slash,      // "/"
    l_parent,   // "("
    r_parent,   // ")"
    semi,       // ";"
    equal,      // =
    comma,      // ,
    eof         // end
*/

llvm::StringRef Token::GetSpellingText(TokenType tokenType)
{
    switch (tokenType)
    {
    case TokenType::kw_int:
        return "int";

    case TokenType::minus:
        return "-";

    case TokenType::plus:
        return "+";

    case TokenType::star:
        return "*";

    case TokenType::slash:
        return "/";

    case TokenType::l_parent:
        return "(";

    case TokenType::r_parent:
        return ")";

    case TokenType::semi:
        return ";";

    case TokenType::equal:
        return "=";

    case TokenType::comma:
        return ",";

    case TokenType::identifier:
        return "identifier";

    case TokenType::number:
        return "number";
    default:
        llvm::llvm_unreachable_internal();
    }
}

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

void Lexer::NextToken(Token &tok)
{

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

    // 2.判断是否结尾
    if (BufPtr >= BufEnd)
    {
        tok.tokenType = TokenType::eof;
        return;
    }

    // 行
    tok.row = row;
    // 列，先处理空格，处理空格完才是真正的token
    tok.col = BufPtr - LineHeadPtr + 1;
    // 记录开始指针
    const char *StartPtr = BufPtr;
    if (IsDigit(*BufPtr))
    {

        int len = 0;
        // 处理数字
        int number = 0;
        while (IsDigit(*BufPtr))
        {
            number = number * 10 + (*BufPtr++ - '0');
        }
        tok.tokenType = TokenType::number;
        tok.value = number;
        tok.ty = CType::GetIntTy();
        tok.ptr = StartPtr;
        tok.len = BufPtr - StartPtr;
    }
    else if (IsLetter(*BufPtr))
    {
        while (IsLetter(*BufPtr) || IsDigit(*BufPtr))
        {
            BufPtr++;
        }
        tok.tokenType = TokenType::identifier;
        tok.ptr = StartPtr;
        tok.len = BufPtr - StartPtr;
        llvm::StringRef text = llvm::StringRef(tok.ptr, tok.len);
        if (text == "int")
        {
            tok.tokenType = TokenType::kw_int;
        }
        else if (text == "if")
        {
            tok.tokenType = TokenType::kw_if;
        }
        else if (text == "else")
        {
            tok.tokenType = TokenType::kw_else;
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
            tok.ptr = StartPtr;
            tok.len = 1;
            break;
        }
        case '-':
        {
            tok.tokenType = TokenType::minus;
            BufPtr++;
            tok.ptr = StartPtr;
            tok.len = 1;
            break;
        }
        case '*':
        {
            tok.tokenType = TokenType::star;
            BufPtr++;
            tok.ptr = StartPtr;
            tok.len = 1;
            break;
        }
        case '/':
        {
            tok.tokenType = TokenType::slash;
            BufPtr++;
            tok.ptr = StartPtr;
            tok.len = 1;
            break;
        }
        case ';':
        {
            tok.tokenType = TokenType::semi;
            BufPtr++;
            tok.ptr = StartPtr;
            tok.len = 1;
            break;
        }
        case '(':
        {
            tok.tokenType = TokenType::l_parent;
            BufPtr++;
            tok.ptr = StartPtr;
            tok.len = 1;
            break;
        }
        case ')':
        {
            tok.tokenType = TokenType::r_parent;
            BufPtr++;
            tok.ptr = StartPtr;
            tok.len = 1;
            break;
        }
        case '=':
        {
            tok.tokenType = TokenType::equal;
            BufPtr++;
            tok.ptr = StartPtr;
            tok.len = 1;
            break;
        }
        case ',':
        {
            tok.tokenType = TokenType::comma;
            BufPtr++;
            tok.ptr = StartPtr;
            tok.len = 1;
            break;
        }
        case '{':
        {
            tok.tokenType = TokenType::l_brace;
            BufPtr++;
            tok.ptr = StartPtr;
            tok.len = 1;
            break;
        }
        case '}':
        {
            tok.tokenType = TokenType::r_brace;
            BufPtr++;
            tok.ptr = StartPtr;
            tok.len = 1;
            break;
        }
        default:
        {
            // llvm::errs() << "unknow char" << *BufPtr << "\n";
            diagEngine.Report(llvm::SMLoc::getFromPointer(BufPtr), diag::err_unknown_char, *BufPtr);
            break;
        }
        }
    }
}

void Lexer::SaveState()
{
    state.BufPtr = BufPtr;
    state.LineHeadPtr = LineHeadPtr;
    state.BufEnd = BufEnd;
    state.row = row;
}

void Lexer::RestoreState()
{
    BufPtr = state.BufPtr;
    BufEnd = state.BufEnd;
    LineHeadPtr = state.LineHeadPtr;
    row = state.row;
}