
cmake_minimum_required(VERSION 3.20)

project(PFML VERSION 0.3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(pfml
    src/main.cpp
    src/lexer/Lexer.cpp
    src/parser/Parser.cpp
    src/runtime/Runtime.cpp
    src/tooling/LanguageDefinition.cpp
)

target_include_directories(pfml PRIVATE src)

if(MSVC)
    target_compile_options(pfml PRIVATE /W4 /EHsc)
else()
    target_compile_options(pfml PRIVATE -Wall -Wextra -Wpedantic)
endif()
