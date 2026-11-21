
#pragma once

#include "../ast/AST.hpp"

#include <string>
#include <unordered_map>
#include <variant>

using Value = std::variant<double, std::string, bool>;

struct Variable
{
    Value value;
    bool isConst;
};

class Runtime
{
public:
    void run(const Program &program);

private:
    std::unordered_map<std::string, Variable> variables;

    Value evaluate(const Expression *expression);
    void execute(const Statement *statement);

    std::string valueToString(const Value &value);
    bool isTruthy(const Value &value);
};
