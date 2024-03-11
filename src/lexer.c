#include <string.h>

#include "common.h"
#include "lexer.h"

void
initLexer(Lexer *l, const char *src)
{
    l->start = src;
    l->current = src;
    l->line = 1;
}

static bool
isDigit(char c)
{
    return c >= '0' && c <= '9';
}

static bool
isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

static bool
isAtEnd(Lexer *l)
{
    return *l->current == '\0';
}

static char
advance(Lexer *l)
{
    l->current++;
    return l->current[-1];
}

static char
peek(Lexer *l)
{
    return *l->current;
}

static char
peekNext(Lexer *l)
{
    if (isAtEnd(l)) return '\0';
    return l->current[1];
}

static bool
match(Lexer *l, char expected)
{
    if (isAtEnd(l)) return false;
    if (*l->current != expected) return false;

    l->current++;
    return true;
}

static Token
makeToken(Lexer *l, TokenType type)
{
    Token token;
    token.type = type;
    token.start = l->start;
    token.length = (int)(l->current - l->start);
    token.line = l->line;
    return token;
}

static Token
errorToken(Lexer *l, const char *message)
{
    Token token;
    token.type = TK_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = l->line;
    return token;
}

static void
skipWhitespace(Lexer *l)
{
    for (;;) {
        char c = peek(l);
        switch (c) {
            case ' ':
            case '\r':
            case '\t': advance(l); break;
            case '\n': {
                l->line++;
                advance(l);
            } break;
            case '/': {
                if (peekNext(l) == '/') {
                    while (peek(l) != '\n' && !isAtEnd(l)) advance(l);
                } else {
                    return;
                }
            } break;
            default: return;
        }
    }
}

static TokenType
checkKeyword(Lexer *l, int start, int length, const char *rest, TokenType type)
{
    if (l->current - l->start == start + length &&
        memcmp(l->start + start, rest, length) == 0) {
        return type;
    }

    return TK_IDENTIFIER;
}

static TokenType
identifierType(Lexer *l)
{
    switch (l->start[0]) {
        case 'a':   return checkKeyword(l, 1, 2, "nd", TK_AND);
        case 'b':   return checkKeyword(l, 1, 4, "reak", TK_BREAK);
        case 'c': {
            if (l->current - l->start > 1) {
                switch (l->start[1]) {
                    case 'l':   return checkKeyword(l, 2, 3, "ass", TK_CLASS);
                    case 'o': {
                        if (l->current - l->start > 1) {
                            switch (l->start[3]) {
                                case 's':   return checkKeyword(l, 4, 1, "t", TK_CONST);
                                case 't':   return checkKeyword(l, 4, 4, "inue", TK_CONTINUE);
                            }
                        }
                    }
                }
            }
        } break;
        case 'd':   return checkKeyword(l, 1, 1, "o", TK_DO);
        case 'e': {
            if (l->current - l->start > 1) {
                switch (l->start[1]) {
                    case 'c':   return checkKeyword(l, 2, 2, "ho", TK_ECHO);
                    case 'l':   return checkKeyword(l, 2, 2, "se", TK_ELSE);
                }
            }
        } break;
        case 'f': {
            if (l->current - l->start > 1) {
                switch (l->start[1]) {
                    case 'a':   return checkKeyword(l, 2, 3, "lse", TK_FALSE);
                    case 'o':   return checkKeyword(l, 2, 1, "r", TK_FOR);
                    case 'n':   return checkKeyword(l, 2, 0, "", TK_FN);
                }
            }
        } break;
        case 'i':   return checkKeyword(l, 1, 1, "f", TK_IF);
        case 'm':   return checkKeyword(l, 1, 5, "odule", TK_MODULE);
        case 'n':   return checkKeyword(l, 1, 3, "ull", TK_NULL);
        case 'o':   return checkKeyword(l, 1, 1, "r", TK_OR);
        case 'r':   return checkKeyword(l, 1, 5, "eturn", TK_RETURN);
        case 's': {
            if (l->current - l->start > 1) {
                switch (l->start[1]) {
                    case 'e':   return checkKeyword(l, 2, 2, "lf", TK_SELF);
                    case 'u':   return checkKeyword(l, 2, 3, "per", TK_SUPER);
                }
            }
        } break;
        case 't':   return checkKeyword(l, 1, 3, "rue", TK_TRUE);
        case 'u':   return checkKeyword(l, 1, 4, "sing", TK_USING);
        case 'v':   return checkKeyword(l, 1, 2, "ar", TK_VAR);
        case 'w':   return checkKeyword(l, 1, 4, "hile", TK_WHILE);
    }

    return TK_IDENTIFIER;
}

static Token
identifier(Lexer *l)
{
    while (isAlpha(peek(l)) || isDigit(peek(l))) advance(l);
    return makeToken(l, identifierType(l));
}

static Token
string(Lexer *l)
{
    while (peek(l) != '"' && !isAtEnd(l)) {
        if (peek(l) == '\n') l->line++;
        advance(l);
    }

    if (isAtEnd(l)) return errorToken(l, "Unterminated String.");

    // Eat the last quote
    advance(l);

    return makeToken(l, TK_STRING);
}

static Token
number(Lexer *l)
{
    while (isDigit(peek(l))) advance(l);

    // Look for a fractional part
    if (peek(l) == '.' && isDigit(peekNext(l))) {
        // Consume the '.'
        advance(l);

        while (isDigit(peek(l))) advance(l);
    }

    return makeToken(l, TK_NUMBER);
}

Token
scanToken(Lexer *l)
{
    skipWhitespace(l);

    l->start = l->current;

    if (isAtEnd(l)) return makeToken(l, TK_EOF);

    char c = advance(l);
    if (isDigit(c)) return number(l);
    if (isAlpha(c)) return identifier(l);

    switch (c) {
        case '(':   return makeToken(l, TK_LPAREN);
        case ')':   return makeToken(l, TK_RPAREN);
        case '{':   return makeToken(l, TK_LBRACE);
        case '}':   return makeToken(l, TK_RBRACE);
        case '[':   return makeToken(l, TK_LBRACK);
        case ']':   return makeToken(l, TK_RBRACK);
        case ';':   return makeToken(l, TK_SEMICOLON);
        case ',':   return makeToken(l, TK_COMMA);
        case '.':   return makeToken(l, TK_DOT);
        case '%':   return makeToken(l, TK_MODULO);
        case '=':   return makeToken(l, match(l, '=') ? TK_EQEQ : TK_EQ);
        case '!':   return makeToken(l, match(l, '=') ? TK_BANGEQ : TK_BANG);
        case '&':   return makeToken(l, TK_BAND);
        case '^':   return makeToken(l, TK_BXOR);
        case '|':   return makeToken(l, TK_BOR);
        case '/': {
            if (match(l, '=')) {
                return makeToken(l, TK_SLASHEQ);
            }
            return makeToken(l, TK_SLASH);
        }
        case '<': {
            if (match(l, '=')) {
                return makeToken(l, TK_LTEQ);
            } else if (match(l, '<')) {
                return makeToken(l, TK_SHL);
            }
            return makeToken(l, TK_LESS);
        }
        case '>': {
            if (match(l, '=')) {
                return makeToken(l, TK_GTEQ);
            } else if (match(l, '>')) {
                return makeToken(l, TK_SHR);
            }
            return makeToken(l, TK_GREATER);
        }
        case '-': {
            if (match(l, '=')) {
                return makeToken(l, TK_MINUSEQ);
            } else if (match(l, '-')) {
                return makeToken(l, TK_DEC);
            }
            return makeToken(l, TK_MINUS);
        }
        case '+': {
            if (match(l, '=')) {
                return makeToken(l, TK_PLUSEQ);
            } else if (match(l, '+')) {
                return makeToken(l, TK_INC);
            }
            return makeToken(l, TK_PLUS);
        }
        case '*': {
            if (match(l, '=')) {
                return makeToken(l, TK_STAREQ);
            } else if (match(l, '*')) {
                return makeToken(l, TK_POWER);
            }
            return makeToken(l, TK_STAR);
        }
        case '"':   return string(l);
    }

    return errorToken(l, "Unexpected Character");
}
