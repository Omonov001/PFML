
#pragma once

#include "../lexer/Lexer.hpp"
#include "../ast/AST.hpp"

#include <vector>
#include <string>

class Parser
{
public:
    explicit Parser(const std::vector<Token> &tokens);

    Program parse();

private:
    const std::vector<Token> &tokens;
    std::size_t current = 0;

    const Token &peek() const;
    const Token &previous() const;
    const Token &advance();

    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    const Token &consume(TokenType type, const std::string &message);

    StmtPtr declaration();
    StmtPtr variableDeclaration(bool isConst);
    StmtPtr statement();
    StmtPtr printStatement();
    StmtPtr ifStatement();
    StmtPtr ifStatementTail();
    StmtPtr blockStatement();

    ExprPtr expression();
    ExprPtr logicalOr();
    ExprPtr logicalAnd();
    ExprPtr equality();
    ExprPtr comparison();
    ExprPtr term();
    ExprPtr factor();
    ExprPtr unary();
    ExprPtr primary();

    [[noreturn]] void error(
        const Token &token,
        const std::string &message);
};
