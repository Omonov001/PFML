
#include "LanguageDefinition.hpp"

const std::vector<KeywordDefinition> &LanguageDefinition::keywords()
{
    static const std::vector<KeywordDefinition> definitions =
        {
            {"let",
             TokenType::Let,
             "Declares a mutable variable.",
             "let name = value;"},
            {"const",
             TokenType::Const,
             "Declares an immutable variable.",
             "const name = value;"},
            {"print",
             TokenType::Print,
             "Prints a value to the console.",
             "print(value);"},
            {"input",
             TokenType::Input,
             "Reads text from the user.",
             "input(\"Enter value: \")"},
            {"if",
             TokenType::If,
             "Executes code when a condition is true.",
             "if (condition) {\n    print(\"Success!\");\n}"},
            {"else",
             TokenType::Else,
             "Executes code when the previous condition is false.",
             "else {\n    print(\"Failed!\");\n}"},
            {"elseif",
             TokenType::ElseIf,
             "Checks another condition after if.",
             "elseif (condition) {\n    print(\"Another condition!\");\n}"},
            {"and",
             TokenType::And,
             "Logical AND operator.",
             "if (a and b) {\n    print(\"Both true!\");\n}"},
            {"or",
             TokenType::Or,
             "Logical OR operator.",
             "if (a or b) {\n    print(\"At least one true!\");\n}"},
            {"true",
             TokenType::True,
             "Boolean true value.",
             "true"},
            {"false",
             TokenType::False,
             "Boolean false value.",
             "false"}};

    return definitions;
}

const KeywordDefinition *LanguageDefinition::findKeyword(
    const std::string &name)
{
    for (const auto &keyword : keywords())
    {
        if (keyword.name == name)
            return &keyword;
    }

    return nullptr;
}

bool LanguageDefinition::isKeyword(const std::string &name)
{
    return findKeyword(name) != nullptr;
}
