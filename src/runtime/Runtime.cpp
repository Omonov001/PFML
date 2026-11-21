
#include "Runtime.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

std::string Runtime::valueToString(const Value &value)
{
    if (std::holds_alternative<double>(value))
    {
        std::ostringstream stream;
        stream << std::get<double>(value);
        return stream.str();
    }

    if (std::holds_alternative<std::string>(value))
        return std::get<std::string>(value);

    return std::get<bool>(value) ? "true" : "false";
}

bool Runtime::isTruthy(const Value &value)
{
    if (std::holds_alternative<bool>(value))
        return std::get<bool>(value);

    if (std::holds_alternative<double>(value))
        return std::get<double>(value) != 0.0;

    return !std::get<std::string>(value).empty();
}

Value Runtime::evaluate(const Expression *expression)
{
    if (auto literal = dynamic_cast<const LiteralExpr *>(expression))
    {
        if (literal->isString)
            return literal->value;

        return std::stod(literal->value);
    }

    if (auto boolean = dynamic_cast<const BooleanExpr *>(expression))
        return boolean->value;

    if (auto variable = dynamic_cast<const VariableExpr *>(expression))
    {
        auto it = variables.find(variable->name);

        if (it == variables.end())
        {
            throw std::runtime_error(
                "Undefined variable: " + variable->name);
        }

        return it->second.value;
    }

    if (auto input = dynamic_cast<const InputExpr *>(expression))
    {
        Value prompt = evaluate(input->prompt.get());

        std::cout << valueToString(prompt);

        std::string result;

        if (!std::getline(std::cin, result))
        {
            throw std::runtime_error(
                "Failed to read input.");
        }

        return result;
    }

    if (auto unary = dynamic_cast<const UnaryExpr *>(expression))
    {
        Value right = evaluate(unary->right.get());

        if (unary->op == "!")
            return !isTruthy(right);

        if (unary->op == "-")
        {
            if (!std::holds_alternative<double>(right))
            {
                throw std::runtime_error(
                    "Unary '-' operator requires a number.");
            }

            return -std::get<double>(right);
        }

        throw std::runtime_error(
            "Unknown unary operator: " + unary->op);
    }

    if (auto binary = dynamic_cast<const BinaryExpr *>(expression))
    {
        const std::string &op = binary->op;

        // Logical AND: short-circuit evaluation.
        if (op == "&&" || op == "and")
        {
            Value left = evaluate(binary->left.get());

            if (!isTruthy(left))
                return false;

            Value right = evaluate(binary->right.get());
            return isTruthy(right);
        }

        // Logical OR: short-circuit evaluation.
        if (op == "||" || op == "or")
        {
            Value left = evaluate(binary->left.get());

            if (isTruthy(left))
                return true;

            Value right = evaluate(binary->right.get());
            return isTruthy(right);
        }

        Value left = evaluate(binary->left.get());
        Value right = evaluate(binary->right.get());

        if (op == "+")
        {
            if (std::holds_alternative<double>(left) &&
                std::holds_alternative<double>(right))
            {
                return std::get<double>(left) +
                       std::get<double>(right);
            }

            if (std::holds_alternative<std::string>(left) &&
                std::holds_alternative<std::string>(right))
            {
                return std::get<std::string>(left) +
                       std::get<std::string>(right);
            }

            throw std::runtime_error(
                "The '+' operator requires two numbers or two strings.");
        }

        if (op == "==" || op == "!=")
        {
            bool equal = false;

            if (left.index() == right.index())
            {
                if (std::holds_alternative<double>(left))
                {
                    equal = std::get<double>(left) ==
                            std::get<double>(right);
                }
                else if (std::holds_alternative<std::string>(left))
                {
                    equal = std::get<std::string>(left) ==
                            std::get<std::string>(right);
                }
                else
                {
                    equal = std::get<bool>(left) ==
                            std::get<bool>(right);
                }
            }

            return op == "==" ? equal : !equal;
        }

        if (!std::holds_alternative<double>(left) ||
            !std::holds_alternative<double>(right))
        {
            throw std::runtime_error(
                "Operator '" + op +
                "' requires two numbers.");
        }

        double a = std::get<double>(left);
        double b = std::get<double>(right);

        if (op == "-")
            return a - b;

        if (op == "*")
            return a * b;

        if (op == "/")
        {
            if (b == 0.0)
                throw std::runtime_error("Division by zero.");

            return a / b;
        }

        if (op == ">")
            return a > b;

        if (op == "<")
            return a < b;

        if (op == ">=")
            return a >= b;

        if (op == "<=")
            return a <= b;

        throw std::runtime_error(
            "Unknown binary operator: " + op);
    }

    throw std::runtime_error("Unknown expression.");
}

void Runtime::execute(const Statement *statement)
{
    if (auto declaration =
            dynamic_cast<const VariableDecl *>(statement))
    {
        if (variables.find(declaration->name) != variables.end())
        {
            throw std::runtime_error(
                "Variable already exists: " + declaration->name);
        }

        Value value = evaluate(declaration->initializer.get());

        variables.emplace(
            declaration->name,
            Variable{value, declaration->isConst});

        return;
    }

    if (auto print = dynamic_cast<const PrintStmt *>(statement))
    {
        Value value = evaluate(print->expression.get());

        std::cout << valueToString(value) << '\n';
        return;
    }

    if (auto block = dynamic_cast<const BlockStmt *>(statement))
    {
        for (const auto &child : block->statements)
            execute(child.get());

        return;
    }

    if (auto ifStmt = dynamic_cast<const IfStmt *>(statement))
    {
        Value condition = evaluate(ifStmt->condition.get());

        if (isTruthy(condition))
        {
            execute(ifStmt->thenBranch.get());
        }
        else if (ifStmt->elseBranch)
        {
            execute(ifStmt->elseBranch.get());
        }

        return;
    }

    throw std::runtime_error("Unknown statement.");
}

void Runtime::run(const Program &program)
{
    for (const auto &statement : program.statements)
        execute(statement.get());
}
