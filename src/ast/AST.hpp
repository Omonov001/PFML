
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <utility>

struct Expression
{
    virtual ~Expression() = default;
};

struct Statement
{
    virtual ~Statement() = default;
};

using ExprPtr = std::unique_ptr<Expression>;
using StmtPtr = std::unique_ptr<Statement>;

struct LiteralExpr : Expression
{
    std::string value;
    bool isString;

    LiteralExpr(std::string value, bool isString)
        : value(std::move(value)), isString(isString) {}
};

struct BooleanExpr : Expression
{
    bool value;

    explicit BooleanExpr(bool value)
        : value(value) {}
};

struct VariableExpr : Expression
{
    std::string name;

    explicit VariableExpr(std::string name)
        : name(std::move(name)) {}
};

struct BinaryExpr : Expression
{
    ExprPtr left;
    std::string op;
    ExprPtr right;

    BinaryExpr(ExprPtr left, std::string op, ExprPtr right)
        : left(std::move(left)),
          op(std::move(op)),
          right(std::move(right)) {}
};

struct UnaryExpr : Expression
{
    std::string op;
    ExprPtr right;

    UnaryExpr(std::string op, ExprPtr right)
        : op(std::move(op)),
          right(std::move(right)) {}
};

struct InputExpr : Expression
{
    ExprPtr prompt;

    explicit InputExpr(ExprPtr prompt)
        : prompt(std::move(prompt)) {}
};

struct VariableDecl : Statement
{
    std::string name;
    bool isConst;
    ExprPtr initializer;

    VariableDecl(std::string name, bool isConst, ExprPtr initializer)
        : name(std::move(name)),
          isConst(isConst),
          initializer(std::move(initializer)) {}
};

struct PrintStmt : Statement
{
    ExprPtr expression;

    explicit PrintStmt(ExprPtr expression)
        : expression(std::move(expression)) {}
};

struct BlockStmt : Statement
{
    std::vector<StmtPtr> statements;

    explicit BlockStmt(std::vector<StmtPtr> statements)
        : statements(std::move(statements)) {}
};

struct IfStmt : Statement
{
    ExprPtr condition;
    StmtPtr thenBranch;
    StmtPtr elseBranch;

    IfStmt(ExprPtr condition, StmtPtr thenBranch, StmtPtr elseBranch)
        : condition(std::move(condition)),
          thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {}
};

struct Program
{
    std::vector<StmtPtr> statements;
};
