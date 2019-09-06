#include "Token.hpp"
#include <sstream>

std::string ToString(TokenType type)
{
    switch(type)
    {
        case TokenType::None:
            return "TokenType::None NONE NONE NONE";
        case TokenType::Number:
            return "TokenType::Number";
        case TokenType::String:
            return "TokenType::String";
        case TokenType::ID:
            return "TokenType::ID";
        case TokenType::Plus:
            return "TokenType::Plus";
        case TokenType::Minus:
            return "TokenType::Minus";
        case TokenType::Star:
            return "TokenType::Star";
        case TokenType::Slash:
            return "TokenType::Slash";
        case TokenType::EQ:
            return "TokenType::EQ";
        case TokenType::NE:
            return "TokenType::NE";
        case TokenType::LT:
            return "TokenType::LT";
        case TokenType::LE:
            return "TokenType::LE";
        case TokenType::GT:
            return "TokenType::NE";
        case TokenType::GE:
            return "TokenType::NE";
        case TokenType::LParen:
            return "TokenType::LParen";
        case TokenType::RParen:
            return "TokenType::RParen";
        case TokenType::LBrace:
            return "TokenType::LBrace";
        case TokenType::RBrace:
            return "TokenType::RBrace";
        case TokenType::LBracket:
            return "TokenType::LBracket";
        case TokenType::RBracket:
            return "TokenType::RBracket";
        case TokenType::Assign:
            return "TokenType::Assign";
        case TokenType::Semicolon:
            return "TokenType::Semicolon";
        case TokenType::Colon:
            return "TokenType::Colon";
        case TokenType::Modificator:
            return "TokenType::Modificator";
        case TokenType::KeywordFunction:
            return "TokenType::KeywordFunction";
        case TokenType::KeywordConst:
            return "TokenType::KeywordConst";
        case TokenType::KeywordBegin:
            return "TokenType::KeywordBegin";
        case TokenType::KeywordEnd:
            return "TokenType::KeywordEnd";
        default:
            return "UNKNOWN";
    }
}

std::string ToString(const Token& t)
{
    std::ostringstream result;
    if (t.data == "")
        result << '[' << t.line << ", " << t.column << ']' << ' ' << '(' << ToString(t.type) << ')';
    else
        result << '[' << t.line << ", " << t.column << ']' << ' ' << '(' << ToString(t.type) << ", " << t.data << ')';
    return result.str();
}
