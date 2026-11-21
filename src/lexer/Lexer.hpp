
#pragma once

#include <string>
#include <vector>

enum class TokenType
{
    Let,
    Const,
    Print,
    If,
    Else,
    ElseIf,
    Input,
    True,
    False,

    And,
    Or,

    Identifier,
    Number,
    String,

    Equal,
    EqualEqual,
    Bang,
    BangEqual,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,

    Plus,
    Minus,
    Star,
    Slash,

    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    Semicolon,

    EndOfFile,
    Unknown
};

struct Token
{
    TokenType type;
    std::string value;
    int line;
    int column;
};

class Lexer
{
public:
    explicit Lexer(const std::string &source);

    std::vector<Token> tokenize();

private:
    std::string source;
    std::vector<Token> tokens;

    std::size_t position = 0;
    int line = 1;
    int column = 1;

    char current() const;
    char advance();
    char peek() const;

    void skipWhitespace();
    void scanIdentifier();
    void scanNumber();
    void scanString();
    void addToken(TokenType type, const std::string &value);

    bool isAtEnd() const;
};

std::string tokenTypeName(TokenType type);
