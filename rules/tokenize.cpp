#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <algorithm>

enum class TokenType
{
    KEYWORD,
    IDENTIFIER,
    STRING,
    NUMBER,
    OPERATOR,
    PUNCTUATOR,
    COMMENT,
    NEWLINE, // Add NEWLINE token type
    INVALID
};

struct Token
{
    TokenType type;
    std::string value;
    size_t line;
    size_t column;

    Token(TokenType type, const std::string &value, size_t line, size_t column)
        : type(type), value(value), line(line), column(column) {}

    std::string toString() const
    {
        return value;
    }
};

bool isKeyword(const std::string &str, const std::vector<std::string> &keywords)
{
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    for (const auto &keyword : keywords)
    {
        if (lowerStr == keyword)
        {
            return true;
        }
    }
    return false;
}

std::vector<Token> tokenize(const std::string &input)
{
    std::vector<Token> tokens;
    size_t line = 1;
    size_t column = 1;

    const std::vector<std::string> keywords = {
        "all", "and", "any", "ascii", "at", "base64", "base64wide", "condition",
        "contains", "endswith", "entrypoint", "false", "filesize", "for",
        "fullword", "global", "import", "icontains", "iendswith", "iequals",
        "in", "include", "int16", "int16be", "int32", "int32be", "int8",
        "int8be", "istartswith", "matches", "meta", "nocase", "none", "not",
        "of", "or", "private", "rule", "startswith", "strings", "them",
        "true", "uint16", "uint16be", "uint32", "uint32be", "uint8",
        "uint8be", "wide", "xor", "defined"};

    for (size_t i = 0; i < input.length();)
    {
        char c = input[i];

        if (c == '\n')
        { // Specifically handle newline characters
            tokens.emplace_back(TokenType::NEWLINE, "\n", line, column);
            line++;
            column = 1;
            i++;
            continue;
        }

        if (std::isspace(c))
        {
            size_t start = i;
            while (i < input.length() && std::isspace(input[i]) && input[i] != '\n')
            {
                column++;
                i++;
            }
            // Do nothing - skip other whitespace
            continue;
        }
        else if (std::isalpha(c) || c == '_')
        {
            size_t start = i;
            while (i < input.length() && (std::isalnum(input[i]) || input[i] == '_'))
            {
                i++;
            }
            std::string identifier = input.substr(start, i - start);
            if (isKeyword(identifier, keywords))
            {
                tokens.emplace_back(TokenType::KEYWORD, identifier, line, column);
            }
            else
            {
                tokens.emplace_back(TokenType::IDENTIFIER, identifier, line, column);
            }
            column += (i - start);
        }
        else if (std::isdigit(c))
        {
            size_t start = i;
            while (i < input.length() && std::isdigit(input[i]))
            {
                i++;
            }
            tokens.emplace_back(TokenType::NUMBER, input.substr(start, i - start), line, column);
            column += (i - start);
        }
        else if (c == '"')
        {
            size_t start = i + 1;
            i++;
            while (i < input.length() && input[i] != '"')
            {
                if (input[i] == '\\' && i + 1 < input.length())
                {
                    i++;
                }
                i++;
            }
            if (i < input.length() && input[i] == '"')
            {
                tokens.emplace_back(TokenType::STRING, input.substr(start, i - start), line, column);
                column += (i - start + 1);
                i++;
            }
            else
            {
                tokens.emplace_back(TokenType::INVALID, input.substr(start - 1, i - start + 1), line, column - 1);
                column += (i - start);
                i = input.length();
            }
        }
        else if (c == '/')
        {
            if (i + 1 < input.length() && input[i + 1] == '/')
            {
                size_t start = i;
                while (i < input.length() && input[i] != '\n')
                {
                    i++;
                }
                // Do nothing - skip single line comments
                if (i < input.length())
                {
                    line++;
                    column = 1;
                    i++;
                }
                continue;
            }
            else if (i + 1 < input.length() && input[i + 1] == '*')
            {
                size_t start = i;
                i += 2;
                while (i + 1 < input.length() && !(input[i] == '*' && input[i + 1] == '/'))
                {
                    if (input[i] == '\n')
                    {
                        line++;
                        column = 1;
                    }
                    else
                    {
                        column++;
                    }
                    i++;
                }
                if (i + 1 < input.length())
                {
                    // Do nothing - skip multiline comments
                    i += 2;
                }
                else
                {
                    tokens.emplace_back(TokenType::INVALID, input.substr(start, i - start), line, column);
                    i = input.length();
                }
                continue;
            }
            else
            {
                tokens.emplace_back(TokenType::OPERATOR, std::string(1, c), line, column);
                column++;
                i++;
            }
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '!' || c == '<' || c == '>')
        {
            tokens.emplace_back(TokenType::OPERATOR, std::string(1, c), line, column);
            column++;
            i++;
        }
        else if (c == '(' || c == ')' || c == '{' || c == '}' || c == ',' || c == ':')
        {
            tokens.emplace_back(TokenType::PUNCTUATOR, std::string(1, c), line, column);
            column++;
            i++;
        }
        else
        {
            tokens.emplace_back(TokenType::INVALID, std::string(1, c), line, column);
            column++;
            i++;
        }
    }

    return tokens;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream inputFile(filename);

    if (!inputFile.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 1;
    }

    std::string input((std::istreambuf_iterator<char>(inputFile)),
                      (std::istreambuf_iterator<char>()));

    inputFile.close();

    std::vector<Token> tokens = tokenize(input);

    for (const auto &token : tokens)
    {
        std::cout << token.toString() << std::endl;
    }

    return 0;
}