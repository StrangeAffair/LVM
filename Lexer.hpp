#ifndef _LEXER_HPP_
#define _LEXER_HPP_

#include "Token.hpp"

#include <string>
#include <vector>

class Lexer{
    std::string        input;
    std::vector<Token> output;
    size_t position;
    size_t length;
private:
    void AddToken(TokenType type, size_t line, size_t column)
    {output.push_back(Token(type, line, column));}
    void AddToken(TokenType type, std::string value, size_t line, size_t column)
    {output.push_back(Token(type, value, line, column));}
private:
    bool Match(const std::string& word);
    bool GetNumber(std::string& result);
    bool GetString(std::string& result);
    bool GetID    (std::string& result);
public:
    Lexer(std::string program)
    {
        this->input    = program;
        this->position = 0;
        this->length   = input.length();
    }
public:
    std::vector<Token> Tokenize();
};

#endif // _LEXER_HPP_

