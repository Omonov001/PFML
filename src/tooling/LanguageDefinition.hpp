
#pragma once

#include "../lexer/Lexer.hpp"

#include <string>
#include <vector>

struct KeywordDefinition
{
    std::string name;
    TokenType tokenType;
    std::string description;
    std::string snippet;
};

class LanguageDefinition
{
public:
    static const std::vector<KeywordDefinition> &keywords();

    static const KeywordDefinition *findKeyword(
        const std::string &name);

    static bool isKeyword(const std::string &name);
};
