#include "lexer.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/ErrorOr.h"
#include "parser.h"
#include "printVisitor.h"
#include "codegen.h"
#include "sema.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("please input filename:\n");
        return 0;
    }

    const char *flie_name = argv[1];
    static llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buf = llvm::MemoryBuffer::getFile(flie_name);
    if (!buf)
    {
        llvm::errs() << "can't open file!\n";
        return -1;
    }
    std::unique_ptr<llvm::MemoryBuffer> membuf = std::move(*buf);
    Lexer lexer(membuf->getBuffer());

    /* Token tok;
    while (true)
    {
        lexer.NextToken(tok);
        if (tok.tokenType != TokenType::eof)
        {
            tok.Dump();
        }
        else
        {
            break;
        }
    } */
    Sema sema;
    Parser parser(lexer, sema);
    auto program = parser.ParseProgram();
    // PrintVisitor printVisitor(program);
    CodeGen codeGen(program);

    return 0;
}