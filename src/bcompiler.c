#include "common.h"
#include "bcompiler.h"
#include "lexer.h"

void
compile(VM *vm, const char *src)
{
    Lexer l;
    initLexer(&l, src);

    int line = -1;
    for (;;) {
        Token token = scanToken(&l);
        if (token.line != line) {
            printf("%4d ", token.line);
            line = token.line;
        } else {
            printf("   | ");
        }
        printf("%2d '%.*s'\n", token.type, token.length, token.start);

        if (token.type == TK_EOF) break;
    }
}
