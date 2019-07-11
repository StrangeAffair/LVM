#ifndef __Token_Hpp__
#define __Token_Hpp__ 1

#include <string>

enum class TokenType
{
    None      = 0,
    Number    = 1,
    String    = 2,
    ID        = 3,

    Plus      = 10,
    Minus     = 11,
    Star      = 12,
    Slash     = 13,

    EQ        = 20,
    NE        = 21,
    LT        = 22,
    LE        = 23,
    GT        = 24,
    GE        = 25,

    LParen    = 30, //(
    RParen    = 31, //)
    LBrace    = 32, //{
    RBrace    = 33, //}
    LBracket  = 34, //[
    RBracket  = 35, //]

    Assign    = 40,
    OldAssign = 41,

    Semicolon = 50,
    Colon     = 51,

    Modificator = 100
};

class Token
{
    TokenType   type;
    std::string data;
public:
    Token()
    {
        this->type = TokenType::None;
        this->data = "";
    }
    Token(TokenType type)
    {
        this->type = type;
        this->data = "";
    }
    Token(TokenType type, std::string text)
    {
        this->type = type;
        this->data = text;
    }
public:
    TokenType   GetType() const
    {return type;}
    std::string GetData() const
    {return data;}
public:
    const Token& operator=(const Token& other)
    {
        if (this == &other)
            return *this;
        this->type = other.type;
        this->data = other.data;
        return *this;
    }
public:
    friend std::string ToString(const Token&);
};

std::string ToString(TokenType type);
std::string ToString(const Token& t);

#endif // TokenType_hpp

