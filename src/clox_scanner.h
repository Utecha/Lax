#ifndef CLOX_SCANNER_H
#define CLOX_SCANNER_H

typedef enum {
    // Single Character Tokens
    TOKEN_LPAREN, TOKEN_RPAREN,
    TOKEN_LBRACE, TOKEN_RBRACE,
    TOKEN_COMMA, TOKEN_DOT,
    TOKEN_MINUS, TOKEN_PLUS,
    TOKEN_SEMICOLON,
    TOKEN_SLASH, TOKEN_STAR,
    TOKEN_NOT, TOKEN_EQ,
    TOKEN_GT, TOKEN_LT,

    // Two Character Tokens
    TOKEN_NOTEQ, TOKEN_EQEQ,
    TOKEN_GTEQ, TOKEN_LTEQ,

    // Literals
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,

    // Keywords
    TOKEN_AND, TOKEN_CLASS,
    TOKEN_ELSE, TOKEN_FALSE,
    TOKEN_FOR, TOKEN_FUN,
    TOKEN_IF, TOKEN_NIL,
    TOKEN_OR, TOKEN_PRINT,
    TOKEN_RETURN, TOKEN_SELF,
    TOKEN_SUPER, TOKEN_TRUE,
    TOKEN_VAR, TOKEN_WHILE,

    // Error / EOF
    TOKEN_ERROR, TOKEN_EOF
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
} Scanner;

void initScanner(Scanner *scanner, const char *source);
Token scanToken(Scanner *scanner);

#endif // CLOX_SCANNER_H
