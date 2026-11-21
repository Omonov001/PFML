
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "runtime/Runtime.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>

int main(int argc, char *argv[])
{
    std::cout << "PFML Language v0.3.0\n";

    if (argc < 2)
    {
        std::cout << "Usage: pfml <file.pfml>\n";
        return 0;
    }

    std::string filePath = argv[1];

    if (
        filePath.size() < 5 ||
        filePath.substr(filePath.size() - 5) != ".pfml")
    {
        std::cerr << "[PFML Error] Expected .pfml file.\n";
        return 1;
    }

    std::ifstream file(filePath);

    if (!file)
    {
        std::cerr << "[PFML Error] Cannot open file: "
                  << filePath << '\n';
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    try
    {
        Lexer lexer(buffer.str());
        auto tokens = lexer.tokenize();

        Parser parser(std::move(tokens));
        Program program = parser.parse();

        Runtime runtime;
        runtime.run(program);
    }
    catch (const std::exception &error)
    {
        std::cerr << "[PFML Error] " << error.what() << '\n';
        return 1;
    }

    return 0;
}
