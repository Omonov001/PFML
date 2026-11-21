
#include "Lexer.hpp"
#include "tooling/LanguageDefinition.hpp"

#include <cctype>

Lexer::Lexer(const std::string &source)
    : source(source) {}

bool Lexer::isAtEnd() const
{
    return position >= source.length();
}

char Lexer::current() const
{
    if (isAtEnd())
        return '\0';

    return source[position];
}

char Lexer::peek() const
{
    if (position + 1 >= source.length())
        return '\0';

    return source[position + 1];
}

char Lexer::advance()
{
    if (isAtEnd())
        return '\0';

    char c = source[position++];

    if (c == '\n')
    {
        line++;
        column = 1;
    }
    else
    {
        column++;
    }

    return c;
}

void Lexer::addToken(TokenType type, const std::string &value)
{
    tokens.push_back({type, value, line, column});
}

void Lexer::skipWhitespace()
{
    while (!isAtEnd())
    {
        char c = current();

        if (std::isspace(static_cast<unsigned char>(c)))
        {
            advance();
        }
        else if (c == '/' && peek() == '/')
        {
            while (!isAtEnd() && current() != '\n')
                advance();
        }
        else
        {
            break;
        }
    }
}

void Lexer::scanIdentifier()
{
    std::string value;

    while (!isAtEnd() &&
           (std::isalnum(static_cast<unsigned char>(current())) ||
            current() == '_'))
    {
        value += advance();
    }

    const KeywordDefinition *keyword =
        LanguageDefinition::findKeyword(value);

    if (keyword != nullptr)
        addToken(keyword->tokenType, value);
    else
        addToken(TokenType::Identifier, value);
}

void Lexer::scanNumber()
{
    std::string value;
    bool hasDecimal = false;

    while (!isAtEnd())
    {
        char c = current();

        if (std::isdigit(static_cast<unsigned char>(c)))
        {
            value += advance();
        }
        else if (c == '.' && !hasDecimal &&
                 std::isdigit(static_cast<unsigned char>(peek())))
        {
            hasDecimal = true;
            value += advance();
        }
        else
        {
            break;
        }
    }

    addToken(TokenType::Number, value);
}

void Lexer::scanString()
{
    advance();

    std::string value;

    while (!isAtEnd() && current() != '"')
    {
        char c = advance();

        if (c == '\\' && !isAtEnd())
        {
            char next = advance();

            if (next == 'n')
                value += '\n';
            else if (next == 't')
                value += '\t';
            else if (next == '"')
                value += '"';
            else if (next == '\\')
                value += '\\';
            else
            {
                value += '\\';
                value += next;
            }
        }
        else
        {
            value += c;
        }
    }

    if (isAtEnd())
    {
        addToken(TokenType::Unknown, "Unterminated string");
        return;
    }

    advance();
    addToken(TokenType::String, value);
}

std::vector<Token> Lexer::tokenize()
{
    while (!isAtEnd())
    {
        skipWhitespace();

        if (isAtEnd())
            break;

        char c = current();

        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_')
        {
            scanIdentifier();
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(c)))
        {
            scanNumber();
            continue;
        }

        if (c == '"')
        {
            scanString();
            continue;
        }

        switch (c)
        {
        case '=':
            advance();
            if (current() == '=')
            {
                advance();
                addToken(TokenType::EqualEqual, "==");
            }
            else
                addToken(TokenType::Equal, "=");
            break;

        case '!':
            advance();
            if (current() == '=')
            {
                advance();
                addToken(TokenType::BangEqual, "!=");
            }
            else
                addToken(TokenType::Bang, "!");
            break;

        case '&':
            advance();
            if (current() == '&')
            {
                advance();
                addToken(TokenType::And, "&&");
            }
            else
                addToken(TokenType::Unknown, "&");
            break;

        case '|':
            advance();
            if (current() == '|')
            {
                advance();
                addToken(TokenType::Or, "||");
            }
            else
                addToken(TokenType::Unknown, "|");
            break;

        case '>':
            advance();
            if (current() == '=')
            {
                advance();
                addToken(TokenType::GreaterEqual, ">=");
            }
            else
                addToken(TokenType::Greater, ">");
            break;

        case '<':
            advance();
            if (current() == '=')
            {
                advance();
                addToken(TokenType::LessEqual, "<=");
            }
            else
                addToken(TokenType::Less, "<");
            break;

        case '+':
            advance();
            addToken(TokenType::Plus, "+");
            break;

        case '-':
            advance();
            addToken(TokenType::Minus, "-");
            break;

        case '*':
            advance();
            addToken(TokenType::Star, "*");
            break;

        case '/':
            advance();
            addToken(TokenType::Slash, "/");
            break;

        case '(':
            advance();
            addToken(TokenType::LeftParen, "(");
            break;

        case ')':
            advance();
            addToken(TokenType::RightParen, ")");
            break;

        case '{':
            advance();
            addToken(TokenType::LeftBrace, "{");
            break;

        case '}':
            advance();
            addToken(TokenType::RightBrace, "}");
            break;

        case ';':
            advance();
            addToken(TokenType::Semicolon, ";");
            break;

        default:
            advance();
            addToken(TokenType::Unknown, std::string(1, c));
            break;
        }
    }

    tokens.push_back({TokenType::EndOfFile, "", line, column});

    return tokens;
}

std::string tokenTypeName(TokenType type)
{
    switch (type)
    {
    case TokenType::Let:
        return "let";
    case TokenType::Const:
        return "const";
    case TokenType::Print:
        return "print";
    case TokenType::If:
        return "if";
    case TokenType::Else:
        return "else";
    case TokenType::ElseIf:
        return "elseif";
    case TokenType::Input:
        return "input";
    case TokenType::True:
        return "true";
    case TokenType::False:
        return "false";

    case TokenType::And:
        return "and / &&";
    case TokenType::Or:
        return "or / ||";

    case TokenType::Identifier:
        return "identifier";
    case TokenType::Number:
        return "number";
    case TokenType::String:
        return "string";

    case TokenType::Equal:
        return "=";
    case TokenType::EqualEqual:
        return "==";
    case TokenType::Bang:
        return "!";
    case TokenType::BangEqual:
        return "!=";
    case TokenType::Greater:
        return ">";
    case TokenType::GreaterEqual:
        return ">=";
    case TokenType::Less:
        return "<";
    case TokenType::LessEqual:
        return "<=";

    case TokenType::Plus:
        return "+";
    case TokenType::Minus:
        return "-";
    case TokenType::Star:
        return "*";
    case TokenType::Slash:
        return "/";

    case TokenType::LeftParen:
        return "(";
    case TokenType::RightParen:
        return ")";
    case TokenType::LeftBrace:
        return "{";
    case TokenType::RightBrace:
        return "}";
    case TokenType::Semicolon:
        return ";";

    case TokenType::EndOfFile:
        return "EOF";
    default:
        return "unknown";
    }
}
