#ifndef _PARSER_HPP_
#define _PARSER_HPP_

#include <vector>
#include <map>
#include "Token.hpp"
#include "Object.hpp"

struct Command
{
    char   data[16];
    size_t size;
};

struct Function
{
    std::string              name;
    std::map<size_t, Object> consts;
    std::vector<Command>     code;
};

class Parser{
    //std::map   <std::string, size_t> MetkaList;
    std::vector<Token>    input;
    std::vector<Function> output;
    size_t position;
    size_t length;
private:
    void skip(size_t n) {position += n;}
private:
    Token get(size_t n);
private:
    bool Compare(TokenType type);
    bool Match(TokenType type);
    bool Match(TokenType type, std::string text);
private:
    bool FindMetka(const Token& t, size_t& dest);
    bool ReadMetka(size_t instuction_number);
private:
    int  ReadPush(Command* dest);
    int  ReadPop (Command* dest);
    int  ReadJump(Command* dest, std::map<std::string, size_t>* MetkaList);
private:
    int ParseCommand   (Command*  dest, std::map<std::string, size_t>* MetkaList);
    int ParseFunction  (Function* dest);
    int ParseModificers(Function* dest);
    int ParseConsts    (std::map<size_t, Object>& const_list);
    int ParseConst     (size_t& offset, Object& value);
    int ParseCode      (std::vector<Command>& code);
public:
    Parser(std::vector<Token> input)
    {
        this->input    = input;
        this->position = 0;
        this->length   = input.size();
    }
    std::vector<Function> Parse();
};

#endif // _PARSER_HPP_
