
#include "Parser.hpp"

#include <stdexcept>
#include <utility>

Parser::Parser(const std::vector<Token> &tokens)
    : tokens(tokens) {}

const Token &Parser::peek() const
{
    return tokens[current];
}

const Token &Parser::previous() const
{
    return tokens[current - 1];
}

bool Parser::isAtEnd() const
{
    return peek().type == TokenType::EndOfFile;
}

const Token &Parser::advance()
{
    if (!isAtEnd())
        current++;

    return previous();
}

bool Parser::check(TokenType type) const
{
    if (isAtEnd())
        return type == TokenType::EndOfFile;

    return peek().type == type;
}

bool Parser::match(TokenType type)
{
    if (!check(type))
        return false;

    advance();
    return true;
}

const Token &Parser::consume(
    TokenType type,
    const std::string &message)
{
    if (check(type))
        return advance();

    error(peek(), message);
}

[[noreturn]] void Parser::error(
    const Token &token,
    const std::string &message)
{
    throw std::runtime_error(
        "Parser error at line " +
        std::to_string(token.line) +
        ", column " +
        std::to_string(token.column) +
        ": " + message);
}

Program Parser::parse()
{
    Program program;

    while (!isAtEnd())
    {
        program.statements.push_back(declaration());
    }

    return program;
}

StmtPtr Parser::declaration()
{
    if (match(TokenType::Let))
        return variableDeclaration(false);

    if (match(TokenType::Const))
        return variableDeclaration(true);

    return statement();
}

StmtPtr Parser::variableDeclaration(bool isConst)
{
    const Token &name = consume(
        TokenType::Identifier,
        "Variable name expected.");

    consume(
        TokenType::Equal,
        "'=' expected after variable name.");

    ExprPtr initializer = expression();

    consume(
        TokenType::Semicolon,
        "';' expected after variable declaration.");

    return std::make_unique<VariableDecl>(
        name.value,
        isConst,
        std::move(initializer));
}

StmtPtr Parser::statement()
{
    if (match(TokenType::Print))
        return printStatement();

    if (match(TokenType::If))
        return ifStatement();

    if (match(TokenType::LeftBrace))
        return blockStatement();

    error(peek(), "Unknown statement.");
}

StmtPtr Parser::printStatement()
{
    consume(TokenType::LeftParen, "'(' expected after print.");

    ExprPtr value = expression();

    consume(TokenType::RightParen, "')' expected after print value.");
    consume(TokenType::Semicolon, "';' expected after print.");

    return std::make_unique<PrintStmt>(std::move(value));
}

StmtPtr Parser::ifStatement()
{
    return ifStatementTail();
}

StmtPtr Parser::ifStatementTail()
{
    consume(TokenType::LeftParen, "'(' expected after if/elseif.");

    ExprPtr condition = expression();

    consume(
        TokenType::RightParen,
        "')' expected after condition.");

    consume(
        TokenType::LeftBrace,
        "'{' expected after if condition.");

    StmtPtr thenBranch = blockStatement();
    StmtPtr elseBranch = nullptr;

    if (match(TokenType::ElseIf))
    {
        elseBranch = ifStatementTail();
    }
    else if (match(TokenType::Else))
    {
        if (match(TokenType::If))
        {
            elseBranch = ifStatementTail();
        }
        else if (match(TokenType::LeftBrace))
        {
            elseBranch = blockStatement();
        }
        else
        {
            error(
                peek(),
                "'{' or 'if' expected after else.");
        }
    }

    return std::make_unique<IfStmt>(
        std::move(condition),
        std::move(thenBranch),
        std::move(elseBranch));
}

StmtPtr Parser::blockStatement()
{
    std::vector<StmtPtr> statements;

    while (!check(TokenType::RightBrace) && !isAtEnd())
    {
        statements.push_back(declaration());
    }

    consume(
        TokenType::RightBrace,
        "'}' expected after block.");

    return std::make_unique<BlockStmt>(std::move(statements));
}

ExprPtr Parser::expression()
{
    return logicalOr();
}

ExprPtr Parser::logicalOr()
{
    ExprPtr expr = logicalAnd();

    while (match(TokenType::Or))
    {
        std::string op = previous().value;
        ExprPtr right = logicalAnd();

        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op,
            std::move(right));
    }

    return expr;
}

ExprPtr Parser::logicalAnd()
{
    ExprPtr expr = equality();

    while (match(TokenType::And))
    {
        std::string op = previous().value;
        ExprPtr right = equality();

        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op,
            std::move(right));
    }

    return expr;
}

ExprPtr Parser::equality()
{
    ExprPtr expr = comparison();

    while (match(TokenType::EqualEqual) ||
           match(TokenType::BangEqual))
    {
        std::string op = previous().value;
        ExprPtr right = comparison();

        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op,
            std::move(right));
    }

    return expr;
}

ExprPtr Parser::comparison()
{
    ExprPtr expr = term();

    while (match(TokenType::Greater) ||
           match(TokenType::GreaterEqual) ||
           match(TokenType::Less) ||
           match(TokenType::LessEqual))
    {
        std::string op = previous().value;
        ExprPtr right = term();

        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op,
            std::move(right));
    }

    return expr;
}

ExprPtr Parser::term()
{
    ExprPtr expr = factor();

    while (match(TokenType::Plus) ||
           match(TokenType::Minus))
    {
        std::string op = previous().value;
        ExprPtr right = factor();

        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op,
            std::move(right));
    }

    return expr;
}

ExprPtr Parser::factor()
{
    ExprPtr expr = unary();

    while (match(TokenType::Star) ||
           match(TokenType::Slash))
    {
        std::string op = previous().value;
        ExprPtr right = unary();

        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op,
            std::move(right));
    }

    return expr;
}

ExprPtr Parser::unary()
{
    if (match(TokenType::Bang) ||
        match(TokenType::Minus))
    {
        std::string op = previous().value;
        ExprPtr right = unary();

        return std::make_unique<UnaryExpr>(
            op,
            std::move(right));
    }

    return primary();
}

ExprPtr Parser::primary()
{
    if (match(TokenType::Number))
    {
        return std::make_unique<LiteralExpr>(
            previous().value,
            false);
    }

    if (match(TokenType::String))
    {
        return std::make_unique<LiteralExpr>(
            previous().value,
            true);
    }

    if (match(TokenType::True))
        return std::make_unique<BooleanExpr>(true);

    if (match(TokenType::False))
        return std::make_unique<BooleanExpr>(false);

    if (match(TokenType::Identifier))
    {
        return std::make_unique<VariableExpr>(
            previous().value);
    }

    if (match(TokenType::Input))
    {
        consume(
            TokenType::LeftParen,
            "'(' expected after input.");

        ExprPtr prompt = expression();

        consume(
            TokenType::RightParen,
            "')' expected after input prompt.");

        return std::make_unique<InputExpr>(
            std::move(prompt));
    }

    if (match(TokenType::LeftParen))
    {
        ExprPtr expr = expression();

        consume(
            TokenType::RightParen,
            "')' expected after expression.");

        return expr;
    }

    error(peek(), "Expression expected.");
}
