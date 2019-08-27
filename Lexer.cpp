#include "Lexer.hpp"

#include <string>

bool Lexer::Match(const std::string& word)
{
    std::string next = input.substr(position, word.length());
    if (next != word)
        return false;
    char ws = input[position + word.length()];
    if ((ws == ' ') || (ws == '\t') || (ws == '\n'))
    {
        position += word.length();
        return true;
    }
    else
    {
        return false;
    }
}

bool Lexer::GetNumber(std::string& result)
{
    result = "";

    char current = input[position];
    if (!isdigit(current)) return false;

    while(true)
    {
        current = input[position];
        if (!isdigit(current)) break;
        result += current;
        ++position;
    }
    return true;
}

bool Lexer::GetString(std::string& result)
{
    result = "";

    char current = input[position];
    if (current != '"') return false;

    ++position;
    while(true)
    {
        current = input[position];
        if (current == '"') break;
        result += current;
        ++position;
    }

    return true;
}

bool Lexer::GetID(std::string& result)
{
    size_t from;
    from   = position;
    result = "";

    char current = input[position];
    switch(current)
    {
        case 'a': case 'A': case 'b': case 'B': case 'c': case 'C':
        case 'd': case 'D': case 'e': case 'E': case 'f': case 'F':
        case 'g': case 'G': case 'h': case 'H': case 'i': case 'I':
        case 'j': case 'J': case 'k': case 'K': case 'l': case 'L':
        case 'm': case 'M': case 'n': case 'N': case 'o': case 'O':
        case 'p': case 'P': case 'q': case 'Q': case 'r': case 'R':
        case 's': case 'S': case 't': case 'T': case 'u': case 'U':
        case 'v': case 'V': case 'w': case 'W': case 'x': case 'X':
        case 'y': case 'Y': case 'z': case 'Z': case '_':
            break;
        default:
            return false;
    }

    while(true)
    {
        current = input[position];
        switch(current)
        {
            case 'a': case 'A': case 'b': case 'B': case 'c': case 'C':
            case 'd': case 'D': case 'e': case 'E': case 'f': case 'F':
            case 'g': case 'G': case 'h': case 'H': case 'i': case 'I':
            case 'j': case 'J': case 'k': case 'K': case 'l': case 'L':
            case 'm': case 'M': case 'n': case 'N': case 'o': case 'O':
            case 'p': case 'P': case 'q': case 'Q': case 'r': case 'R':
            case 's': case 'S': case 't': case 'T': case 'u': case 'U':
            case 'v': case 'V': case 'w': case 'W': case 'x': case 'X':
            case 'y': case 'Y': case 'z': case 'Z': case '_':
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                ++position;
                break;
            default:
                result = input.substr(from, position - from);
                return true;
        }
    }

    return true;
}

std::vector<Token> Lexer::Tokenize()
{
    size_t line   = 1;
    size_t column = 1;
    std::string text = "";
    bool ok;
    while(position < length)
    {
        char current = input[position];
        switch(current)
        {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                ok = GetNumber(text);
                if (ok)
                    AddToken(TokenType::Number, text, line, column);
                column += text.length();
                break;
            case 'a': case 'A': case 'b': case 'B': case 'c': case 'C':
            case 'd': case 'D': case 'e': case 'E': case 'f': case 'F':
            case 'g': case 'G': case 'h': case 'H': case 'i': case 'I':
            case 'j': case 'J': case 'k': case 'K': case 'l': case 'L':
            case 'm': case 'M': case 'n': case 'N': case 'o': case 'O':
            case 'p': case 'P': case 'q': case 'Q': case 'r': case 'R':
            case 's': case 'S': case 't': case 'T': case 'u': case 'U':
            case 'v': case 'V': case 'w': case 'W': case 'x': case 'X':
            case 'y': case 'Y': case 'z': case 'Z': case '_':
                ok = GetID(text);
                if (ok)
                    AddToken(TokenType::ID, text, line, column);
                column += text.length();
                break;
            case '"':
                ok = GetString(text);
                if (ok)
                    AddToken(TokenType::String, text, line, column);
                column += text.length() + 2;
                break;
            case '+':
                AddToken(TokenType::Plus, line, column);
                ++position;
                ++column;
                break;
            case '-':
                AddToken(TokenType::Minus, line, column);
                ++position;
                ++column;
                break;
            case '*':
                AddToken(TokenType::Star, line, column);
                ++position;
                ++column;
                break;
            case '/':
                AddToken(TokenType::Slash, line, column);
                ++position;
                ++column;
                break;
            case '(':
                AddToken(TokenType::LParen, line, column);
                ++position;
                ++column;
                break;
            case ')':
                AddToken(TokenType::RParen, line, column);
                ++position;
                ++column;
                break;
            case '[':
                AddToken(TokenType::LBracket, line, column);
                ++position;
                ++column;
                break;
            case ']':
                AddToken(TokenType::RBracket, line, column);
                ++position;
                ++column;
                break;
            case '{':
                AddToken(TokenType::LBrace, line, column);
                ++position;
                ++column;
                break;
            case '}':
                AddToken(TokenType::RBrace, line, column);
                ++position;
                ++column;
                break;
            case ':':
                if (Match(":="))
                {
                    AddToken(TokenType::Assign, line, column);
                    //position += 2;
                    column += 2;
                    break;
                }
                AddToken(TokenType::Colon, line, column);
                ++position;
                ++column;
                break;
            case ';':
                AddToken(TokenType::Semicolon, line, column);
                ++position;
                ++column;
                break;
            case ' ': case '\t':
                ++position;
                ++column;
                break;
            case '\n':
                ++line;
                ++position;
                column = 1;
                break;
            /*
            case '#':
                ++position;
                ok = GetID(text);
                if (ok)
                    AddToken(TokenType::Modificator, text, line);
                break;
            */
            default:
                AddToken(TokenType::None, std::string(&current, 1), line, column);
                ++position;
                ++column;
                break;
        }
    }
    return output;
}

