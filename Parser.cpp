#include "Parser.hpp"

#include <vector>
#include "Token.hpp"

static bool ParseSize(const Token& t, size_t& dest)
{
    dest = 0;

    size_t length = t.GetData().length();
    for(size_t i = 0; i < length; ++i)
    {
        char current = t.GetData()[i];
        switch(current)
        {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                dest *= 10;
                dest += current - '0';
                break;
            default:
                fprintf(stderr, "ParseSize fail incorrect input symbol");
                return false;
        }
    }
    return true;
}

static bool ParseInt32  (const Token& t, int32_t& dest)
{
    size_t i    = 0;
    char   sign = 1;
    dest = 0;
    if (t.GetData()[0] == '-')
    {
        sign = -1;
        ++i;
    }

    size_t length = t.GetData().length();
    for(; i < length; ++i)
    {
        char current = t.GetData()[i];
        switch(current)
        {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                dest *= 10;
                dest += current - '0';
                break;
            default:
                fprintf(stderr, "ParseSize fail incorrect input symbol");
                return false;
        }
    }

    dest *= sign;
    return true;
}

Token Parser::get(size_t n)
{
    return input[position + n];
}

bool Parser::Compare(TokenType type)
{
    Token current = input[position];
    if (current.GetType() == type)
        return true;
    else
        return false;
}

bool Parser::Match(TokenType type)
{
    if (Compare(type))
    {
        ++position;
        return true;
    }
    else
    {
        return false;
    }
}

bool Parser::Match(TokenType type, std::string text)
{
    Token current = input[position];
    if ((current.GetType() == type) && (current.GetData() == text))
    {
        ++position;
        return true;
    }
    else
    {
        return false;
    }
}

std::vector<Function> Parser::Parse()
{
    int      error = 0;
    Function temp  = {};
    while(position < length)
    {
        error = ParseFunction(&temp);
        if (error)
        {
            fprintf(stderr, "something went wrong\n");
            return output;
        }
        output.push_back(temp);
    }
    return output;
}

int Parser::ParseFunction(Function* dest)
{
    if (!Match(TokenType::ID, "function"))
        return 1;

    size_t start = position;

    Token current = input[position];
    ++position;
    if (current.GetType() != TokenType::ID)
    {
        position = start;
        return 2;
    }
    dest->name = current.GetData();

    if (!Match(TokenType::LParen))
    {
        position = start;
        return 3;
    }
    if (!Match(TokenType::RParen))
    {
        position = start;
        return 4;
    }
    if (!Match(TokenType::Colon))
    {
        position = start;
        return 5;
    }

    if (ParseModificers(dest))
    {
        position = start;
        return 6;
    }
    return 0;
}

int Parser::ParseModificers(Function* dest)
{
    int error = 0;
    while(position < length)
    {
        if (Match(TokenType::ID, "end"))
            break;
        if (Match(TokenType::Modificator, "consts"))
        {
            error = Parser::ParseConsts(dest->consts);
            if (error) fprintf(stderr, "ParseConsts fail");
            continue;
        }
        if (Match(TokenType::Modificator, "code"))
        {
            error = Parser::ParseCode  (dest->code);
            if (error) fprintf(stderr, "ParseCode fail");
            continue;
        }
        return 1;
    }
    return 0;
}

int Parser::ParseConst(size_t& offset, Object& value)
{
    if (!Match(TokenType::ID, "const"))
    {
        fprintf(stderr, "ParseConst fail: no const keyword");
        return 1;
    }

    if (!Match(TokenType::LBracket))
    {
        fprintf(stderr, "ParseConsts fail: no '[' symbol");
        return 2;
    }

    bool ok;
    ok = ParseSize(input[position++], offset);
    if (!ok)
        return 3;

    if (!Match(TokenType::RBracket))
    {
        fprintf(stderr, "ParseConsts fail: no '] symbol");
        return 4;
    }

    if (!Match(TokenType::Assign))
    {
         fprintf(stderr, "ParseConsts fail: no assign");
        return 5;
    }

    int32_t obj_value;
    ok = ParseInt32(input[position++], obj_value);
    if (!ok)
        return 6;
    Object::set(value, obj_value);
    return 0;
}

int Parser::ParseConsts(std::map<size_t, Object>& consts)
{
    int line  = 1;
    int error = 0;

    size_t offset;
    Object temp;
    while(true)
    {
        if (Compare(TokenType::Modificator))
            return 0;

        error = ParseConst(offset, temp);
        if (error)
        {
            fprintf(stderr, "in line %d", line);
            return 1;
        }
        Object::copy(consts[offset], temp);
        ++line;
    }
}

int Parser::ReadPush(Command* dest)
{
    Token current = input[position];
    if (current.GetType() != TokenType::ID)
        return 1;
    if (current.GetData() != "push")
        return 2;

    ++position;
    current = input[position];
    if ((current.GetType() != TokenType::ID))
        return 3;
    if ((current.GetData() != "consts") && (current.GetData() != "memory"))
        return 4;

    if (dest != nullptr)
    {
        if (current.GetData() == "consts")
        {
            dest->data[0] = 1;
        }
        if (current.GetData() == "memory")
        {
            dest->data[0] = 2;
        }
    }

    ++position;
    current = input[position];
    if (current.GetType() != TokenType::LBracket)
        return 5;

    ++position;
    current = input[position];
    if (current.GetType() != TokenType::Number)
        return 6;

    if (dest != nullptr)
    {
        size_t offset;
        ParseSize(current, offset);
        memcpy(dest->data + 1, &offset, sizeof(size_t));
    }

    ++position;
    current = input[position];
    if (current.GetType() != TokenType::RBracket)
        return 7;

    ++position;
    if (dest != nullptr)
        dest->size = 1 + sizeof(size_t);
    return 0;
}

int Parser::ReadPop(Command* dest)
{
    Token current = input[position];
    if (current.GetType() != TokenType::ID)
        return 1;
    if (current.GetData() != "pop")
        return 2;

    ++position;
    current = input[position];
    if (current.GetType() != TokenType::ID)
        return 3;
    if (current.GetData() != "memory")
        return 4;

    if (dest != nullptr)
    {
        if (current.GetData() == "memory")
        {
            dest->data[0] = 3;
        }
    }

    ++position;
    current = input[position];
    if (current.GetType() != TokenType::LBracket)
        return 5;

    ++position;
    current = input[position];
    if (current.GetType() != TokenType::Number)
        return 6;

    if (dest != nullptr)
    {
        size_t offset;
        ParseSize(current, offset);
        memcpy(dest->data + 1, &offset, sizeof(size_t));
    }

    ++position;
    current = input[position];
    if (current.GetType() != TokenType::RBracket)
        return 7;

    ++position;
    if (dest != nullptr)
        dest->size = 1 + sizeof(size_t);
    return 0;
}

int Parser::ReadJump(Command* dest, std::map<std::string, size_t>* MetkaList)
{
    Token current = input[position];
    if ((current.GetType() != TokenType::ID))
        return 1;
    if ((current.GetData() != "jump") && (current.GetData() != "ifjump"))
        return 2;

    size_t jump_type;
    if (current.GetData() == "jump")
        jump_type = 0;
    if (current.GetData() == "ifjump")
        jump_type = 1;

    ++position;
    current = input[position];
    if (current.GetType() != TokenType::ID)
        return 3;

    if (MetkaList == nullptr)
    {
        //all possible checks done
        ++position;
        return 0;
    }

    auto it = MetkaList->find(current.GetData());
    if (it == MetkaList->end())
    {
        fprintf(stderr, "could not find metka");
        return 4;
    }
    size_t offset = it->second;

    if (dest == nullptr)
    {
        ++position;
        return 0;
    }

    if (jump_type == 0)
        dest->data[0] = 0x20;
    if (jump_type == 1)
        dest->data[0] = 0x21;

    memcpy(dest->data + 1, &offset, sizeof(size_t));
    dest->size = 1 + sizeof(size_t);
    ++position;
    return 0;
}

int Parser::ParseCommand(Command* dest, std::map<std::string, size_t>* MetkaList)
{
    Token current = input[position];
    if (current.GetType() != TokenType::ID)
    {
        fprintf(stderr, "Unexpected Token as the name of command");
        return 1;
    }

    std::string CommandName = current.GetData();
    if (CommandName == "push")
        return ReadPush(dest);
    if (CommandName == "pop")
        return ReadPop (dest);
    if (CommandName == "add")
    {
        if (dest != nullptr)
        {
            dest->data[0] = 4;
            dest->size    = 1;
        }
        ++position;
        return 0;
    }
    if (CommandName == "ge")
    {
        if (dest != nullptr)
        {
            dest->data[0] = 0x15;
            dest->size    = 1;
        }
        ++position;
        return 0;
    }
    if ((CommandName == "jump") || (CommandName == "ifjump"))
        return ReadJump(dest, MetkaList);

    fprintf(stderr, "Unexpected name of command\n");
    return 2;
}

int Parser::ParseCode(std::vector<Command>& code)
{
    std::map<std::string, size_t> MetkaList;

    size_t start = position;
    size_t count = 0;
    while(position < length)
    {
        Token current = input[position];
        Token next    = input[position + 1];

        if ((current.GetType() == TokenType::ID) && (next.GetType() == TokenType::Colon))
        {
            MetkaList[current.GetData()] = count;
            position += 2;
        }

        current = input[position];
        if (current.GetType() != TokenType::ID)
            return 1;
        if (current.GetData() == "end")
            break;

        if (ParseCommand(nullptr, nullptr))
            return 2;

        ++count;
    }

    code.resize(count);
    position = start;
    count    = 0;
    while(position < length)
    {
        Token current = input[position];
        Token next    = input[position + 1];

        if ((current.GetType() == TokenType::ID) && (next.GetType() == TokenType::Colon))
            position += 2;

        current = input[position];
        if (current.GetType() != TokenType::ID)
            return 1;
        if (current.GetData() == "end")
            break;

        if (ParseCommand(&code[count], &MetkaList))
            return 3;
        ++count;
    }

    for(size_t i = 0; i < count; ++i)
    {
        for(size_t j = 0; j < code[i].size; ++j)
            fprintf(stdout, "%2x ", code[i].data[j]);
        fprintf(stdout, "\n");
    }

    return 0;
}

/*
Statement* Parser::statement()
{
    if (input[position].GetType() == TokenType::ID)
    {
        if (input[position + 1].GetType() == TokenType::Assign)
            return assignment();
    }
    return new EmptyStatement();
}

Statement* Parser::assignment()
{
    std::string varname = get(0).GetText();
    skip(2);
    Expression* expr    = expression();
    return new AssignmentStatement(varname, expr);
}

Expression* Parser::expression()
{
    return additive();
}

Expression* Parser::additive()
{
    Expression* expr = multiplicative();

    while(true)
    {
        if (Match(TokenType::Plus))
        {
            expr = new BinaryExpression(expr, TokenType::Plus, multiplicative());
            continue;
        }
        if (Match(TokenType::Minus))
        {
            expr = new BinaryExpression(expr, TokenType::Minus, multiplicative());
            continue;
        }
        break;
    }

    return expr;
}

Expression* Parser::multiplicative()
{
    Expression* expr = unary();

    while(true)
    {
        if (Match(TokenType::Star))
        {
            expr = new BinaryExpression(expr, TokenType::Star, unary());
            continue;
        }
        if (Match(TokenType::Slash))
        {
            expr = new BinaryExpression(expr, TokenType::Slash, unary());
            continue;
        }
        break;
    }

    return expr;
}

Expression* Parser::unary()
{
    if (Match(TokenType::Minus))
        return new UnaryExpression(TokenType::Minus, primary());
    return primary();
}

//Type: Block?, retval?
// retval != null => expression
// retval == null => statement
// block == true => expression block || statement block
Expression* Parser::primary()
{
    Token current = get(0);
    if (current.GetType() == TokenType::Number)
    {
        skip(1);
        return new Constant(current.GetText());
    }
    if (current.GetType() == TokenType::ID)
    {
        skip(1);
        return new Variable(current.GetText());
    }
    if (Match(TokenType::LParen))
    {
        Expression* result = expression();
        if (Match(TokenType::RParen))
            return result;
        else
            return new EmptyExpression();
    }
    return new EmptyExpression();
}
*/
