#include <stdio.h>
#include <string.h>

#include "clox_common.h"
#include "clox_scanner.h"

Scanner scanner;

void initScanner(const char *source)
{
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static bool isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

static bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

static bool isAtEnd()
{
    return *scanner.current == '\0';
}

static char advance()
{
    scanner.current++;
    return scanner.current[-1];
}

static char peek()
{
    return *scanner.current;
}

static char peekNext()
{
    if (isAtEnd()) return '\0';
    return scanner.current[1];
}

static bool match(char expected)
{
    if (isAtEnd()) return false;
    if (*scanner.current != expected) return false;
    scanner.current++;
    return true;
}

static Token makeToken(TokenType type)
{
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

static Token errorToken(const char *message)
{
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;
    return token;
}

static void skipWhitespace()
{
    for (;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':  advance(); break;
            case '\n': {
                scanner.line++;
                advance();
            } break;
            case '/': {
                if (peekNext() == '/') {
                    // Comments go until the end of the line
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    return;
                }
            } break;
            default: return;
        }
    }
}

static TokenType checkKeyword(int start, int length, const char *rest, TokenType type)
{
    if (scanner.current - scanner.start == start + length &&
        memcmp(scanner.start + start, rest, length) == 0) {
        // Return the Keyword TokenType if everything matches up
        return type;
    }

    return TOKEN_IDENTIFIER;
}

static TokenType identifierType()
{
    switch (scanner.start[0]) {
        case 'a':   return checkKeyword(1, 2, "nd", TOKEN_AND);
        case 'c':   return checkKeyword(1, 4, "lass", TOKEN_CLASS);
        case 'e':   return checkKeyword(1, 3, "lse", TOKEN_ELSE);
        case 'f': {
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'a':   return checkKeyword(2, 3, "lse", TOKEN_FALSE);
                    case 'o':   return checkKeyword(2, 1, "r", TOKEN_FOR);
                    case 'u':   return checkKeyword(2, 1, "n", TOKEN_FUN);
                }
            }
        } break;
        case 'i':   return checkKeyword(1, 1, "f", TOKEN_IF);
        case 'n':   return checkKeyword(1, 2, "il", TOKEN_NIL);
        case 'o':   return checkKeyword(1, 1, "r", TOKEN_OR);
        case 'p':   return checkKeyword(1, 4, "rint", TOKEN_PRINT);
        case 'r':   return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
        case 's': {
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'e':   return checkKeyword(2, 2, "lf", TOKEN_SELF);
                    case 'u':   return checkKeyword(2, 3, "per", TOKEN_SUPER);
                }
            }
        } break;
        case 't':   return checkKeyword(1, 3, "rue", TOKEN_TRUE);
        case 'v':   return checkKeyword(1, 2, "ar", TOKEN_VAR);
        case 'w':   return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    }

    return TOKEN_IDENTIFIER;
}

static Token identifier()
{
    while (isAlpha(peek()) || isDigit(peek())) advance();
    return makeToken(identifierType());
}

static Token number()
{
    while (isDigit(peek())) advance();

    // Look for the decimal point
    if (peek() == '.' && isDigit(peekNext())) {
        // Consume the '.'
        advance();

        while (isDigit(peek())) advance();
    }

    return makeToken(TOKEN_NUMBER);
}

static Token string()
{
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') scanner.line++;
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated String.");

    // Consume the closing quote
    advance();
    return makeToken(TOKEN_STRING);
}

Token scanToken()
{
    skipWhitespace();
    scanner.start = scanner.current;

    // Make EOF token, marking the end of the file, and return.
    if (isAtEnd()) return makeToken(TOKEN_EOF);

    char c = advance();
    if (isDigit(c)) return number();
    if (isAlpha(c)) return identifier();

    switch (c) {
        case '(':   return makeToken(TOKEN_LPAREN);
        case ')':   return makeToken(TOKEN_RPAREN);
        case '{':   return makeToken(TOKEN_LBRACE);
        case '}':   return makeToken(TOKEN_RBRACE);
        case ';':   return makeToken(TOKEN_SEMICOLON);
        case ',':   return makeToken(TOKEN_COMMA);
        case '.':   return makeToken(TOKEN_DOT);
        case '-':   return makeToken(match('-') ? TOKEN_DECREMENT : TOKEN_MINUS);
        case '+':   return makeToken(match('+') ? TOKEN_INCREMENT : TOKEN_PLUS);
        case '/':   return makeToken(TOKEN_SLASH);
        case '%':   return makeToken(TOKEN_MODULO);
        case '*':   return makeToken(match('*') ? TOKEN_POWER : TOKEN_STAR);
        case '!':   return makeToken(match('=') ? TOKEN_NOTEQ : TOKEN_NOT);
        case '=':   return makeToken(match('=') ? TOKEN_EQEQ : TOKEN_EQ);
        case '>':   return makeToken(match('=') ? TOKEN_GTEQ : TOKEN_GT);
        case '<':   return makeToken(match('=') ? TOKEN_LTEQ : TOKEN_LT);
        case '"':   return string();
    }

    return errorToken("Unexpected Character.");
}
