#ifndef _PARSER_HPP_
#define _PARSER_HPP_

#include <vector>
#include <map>
#include "Token.hpp"
#include "Object.hpp"

using std::map;
using std::string;
using std::vector;

struct Command
{
    char   data[16];
    size_t size;
};


struct ParserFunction
{
    string name;

    size_t start;
    size_t finish; //next after keyword end
    size_t begin;  //keyword begin position

    map<size_t, Object> ConstList;
    map<string, size_t> MetkaList;

    Command* Code;
    size_t   size;
};

struct Function
{
    string name;  //debug field
    map<size_t, Object> consts;
    map<string, size_t> __MetkaList;
    Command* Code;
    size_t   size;
};

class Parser{
    //std::map<std::string, Function> functions;
    //std::map   <std::string, size_t> MetkaList;
    vector<Token>          input;
    vector<Function>       output;
    map   <string, size_t> FunctionSearchList; //to search function
    size_t position;
    size_t length;
private:
    size_t error;
private:
    Token* Get (size_t n);
    Token* Next();
    Token* Skip(size_t n);
private:
    bool Match(TokenType type, const char* error_message);
    bool Match(TokenType type, string data, const char* error_message);
    bool Compare(TokenType type, const char* error_message);
    bool Compare(TokenType type, string data, const char* error_message);
private:
    void AddFunction(string name);
private:
    size_t ReadFunction1();
    size_t ReadFunction2();
private:
    size_t ReadConsts1();
    size_t ReadConsts2(Function* dest);
private:
    size_t ReadConst1();
    size_t ReadConst2(Function* dest);
private:
    size_t ReadCode1(Function* dest);
    size_t ReadCode2(Function* dest);
private:
    size_t ReadCommand1();
    size_t ReadCommand2(Command* dest, const map<string, size_t>& MetkaList, const map<string, size_t>& FunctionList);
private:
    size_t ReadMetka(size_t cmd_number, map<string, size_t>& MetkaList);
private:
    bool CommandCheck();
    bool CommandRead (string name, size_t index, bool MetkaListReady);
private:
    bool IsMetka();
    size_t ReadPush(Command* dest);
    size_t ReadPop (Command* dest);
    size_t ReadJump(Command* dest, const map<string, size_t>& MetkaList);
    size_t ReadCall(Command* dest, const map<string, size_t>& FunctionList);
private:
    size_t FindFunction(string name, const map<string, size_t>& FunctionList) const;
    size_t FindMetka   (string name, const map<string, size_t>& MetkaList)    const;
private:
    void ParseFunction  (size_t look_number);

    size_t ParseConsts(map<size_t, Object>* ConstList);
    size_t ParseConst (map<size_t, Object>* ConstList);

    size_t ParseCode   (ParserFunction* fn);
    size_t ParseCommand(ParserFunction* fn, size_t cmd_number);
public:
    Parser(std::vector<Token> input)
    {
        this->input    = input;
        this->position = 0;
        this->length   = input.size();

        error = 0;
    }
    vector<Function>& Parse();
};

#endif // _PARSER_HPP_
