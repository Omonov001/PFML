
#include "Lexer.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: pfml-lexer <file.pfml>\n";
        return 1;
    }

    std::ifstream file(argv[1]);

    if (!file)
    {
        std::cerr << "Cannot open file.\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    Lexer lexer(buffer.str());
    auto tokens = lexer.tokenize();

    for (const auto &token : tokens)
    {
        std::cout
            << tokenTypeName(token.type)
            << " (" << token.value << ")"
            << " [" << token.line << "]\n";

        if (token.type == TokenType::Unknown)
            return 1;
    }

    return 0;
}
