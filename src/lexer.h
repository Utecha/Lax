#ifndef LAX_LEXER_H
#define LAX_LEXER_H

typedef enum {
    // Symbol Tokens
    TK_LPAREN, TK_RPAREN,
    TK_LBRACE, TK_RBRACE,
    TK_LBRACK, TK_RBRACK,
    TK_COMMA, TK_DOT,
    TK_QUESTION, TK_COLON,
    TK_SEMICOLON,

    // Arithmetic Tokens
    TK_MINUS, TK_PLUS,
    TK_SLASH, TK_STAR,
    TK_MODULUS, TK_POWER,

    // Equality Tokens
    TK_BANG, TK_BANGEQ,
    TK_EQEQ,

    // Comparison Tokens
    TK_GREATER, TK_GTEQ,
    TK_LESS, TK_LTEQ,

    // Bitwise Tokens
    TK_BAND, TK_BOR,
    TK_BXOR, TK_SHL,
    TK_SHR,

    // Reassignment Tokens
    TK_INC, TK_DEC,
    TK_MINUSEQ, TK_PLUSEQ,
    TK_SLASHEQ, TK_STAREQ,
    TK_EQ,

    // Literal Tokens
    TK_IDENTIFIER,
    TK_STRING,
    TK_NUMBER,
    // TK_INTEGER,
    // TK_DOUBLE,

    // Lax Keyword Tokens
    TK_AND, TK_BREAK,
    TK_CLASS, TK_CONST,
    TK_CONTINUE, TK_DO,
    TK_ELSE, TK_ECHO,
    TK_FALSE, TK_FOR,
    TK_FN, TK_IF,
    TK_MODULE, TK_NULL,
    TK_OR, TK_RETURN,
    TK_SELF, TK_SUPER,
    TK_TRUE, TK_USING,
    TK_VAR, TK_WHILE,

    // Special Tokens
    TK_ERROR, TK_EOF, TK_COUNT

} TokenType;

typedef struct {
    TokenType type;
    const char *start;
    int length;
    int line;
} Token;

typedef struct {
    const char *start;
    const char *current;
    int line;
} Lexer;

void
initLexer(Lexer *l, const char *src);

Token
scanToken(Lexer *l);

#endif // LAX_LEXER_H
