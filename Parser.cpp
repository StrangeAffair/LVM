#include "Parser.hpp"

#include <vector>
#include <cinttypes>
#include <cassert>
#include "Token.hpp"

using std::map;
using std::string;
using std::vector;

const size_t DEBUG_LEVEL = 1;

/**
main function Parser::Parse() should return vector<Function>, where

Function =
{
    map<size_t, Object> consts;
    Command*            Code;
    size_t              size;
}

Because we use metkas and do not require functions forward declaration we
should read input at least 2 times. To make it clearly I wrote 2 variants
for each read with digit in the end of function name.
(Example: ReadFunction1 and ReadFunction2)
*/

//functions for reading numbers
static
void ReadSize(const Token* t, Object& dest)
{
    size_t value;
    if (sscanf(t->GetData().c_str(), "%" SCNu32, &value) == 1)
        Object::set(dest, value);
    else
        dest.clear();
}

static
void ReadInt32(const Token* t, Object& dest)
{
    int32_t value;
    if (sscanf(t->GetData().c_str(), "%" SCNd32, &value) == 1)
        Object::set(dest, value);
    else
        dest.clear();
}

static
size_t ParseSize (const Token* t, size_t& dest)
{
    size_t index;
    index = 0;
    dest  = 0;
    for(auto ch : t->GetData())
    {
        switch(ch)
        {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                dest *= 10;
                dest += ch - '0';
                ++index;
                break;
            default:
                return index;
        }
    }
    return index;
}

static
size_t ParseInt32(const Token* t, int32_t& dest)
{
    size_t index;
    char   sign;
    index = 0;
    dest  = 0;
    sign  = 1;
    for(auto ch : t->GetData())
    {
        switch(ch)
        {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                dest *= 10;
                dest += ch - '0';
                ++index;
                break;
            case '-':
                if (index != 0)
                    return index;
                sign = -1;
            case '+':
                if (index != 0)
                    return index;
                sign = 1;
            default:
                return index;
        }
    }
    dest *= sign;
    return index;
}

Token* Parser::Get(size_t n = 0)
{
    if (position + n < length)
        return &input[position + n];
    else
        return nullptr;
}

Token* Parser::Next()
{
    ++position;
    if (position < length)
        return &input[position];
    else
        return nullptr;
}

Token* Parser::Skip(size_t n)
{
    position += n;
    if (position < length)
        return &input[position];
    else
        return nullptr;
}

bool Parser::Match(TokenType type, const char* message)
{
    assert(Get(0)  != nullptr);

    Token* token = Get(0);
    if (token->GetType() == type)
    {
        Next();
        return true;
    }

    if (message != nullptr)
    {
        fprintf(stderr, "%s (%d, %d)\n", message, token->line, token->column);
        error = 1;
    }
    return false;
}

bool Parser::Match(TokenType type, string data, const char* message)
{
    assert(Get(0)  != nullptr);

    Token* token = Get(0);
    if ((token->GetType() == type) && (token->GetData() == data))
    {
        Next();
        return true;
    }

    if (message != nullptr)
    {
        fprintf(stderr, "%s (%d, %d)\n", message, token->line, token->column);
        error = 1;
    }
    return false;
}


bool Parser::Compare(TokenType type, const char* message)
{
    assert(Get(0)  != nullptr);

    Token* token = Get(0);
    if (token->GetType() == type)
    {
        return true;
    }

    if (message != nullptr)
    {
        fprintf(stderr, "%s (%d, %d)\n", message, token->line, token->column);
        error = 1;
    }
    return false;
}

bool Parser::Compare(TokenType type, string data, const char* message)
{
    assert(Get(0)  != nullptr);

    Token* token = Get(0);
    if ((token->GetType() == type) && (token->GetData() == data))
    {
        return true;
    }

    if (message != nullptr)
    {
        fprintf(stderr, "%s (%d, %d)\n", message, token->line, token->column);
        error = 1;
    }
    return false;
}

void Parser::AddFunction(string name)
{
    FunctionSearchList[name] = output.size();
    output.push_back(Function());
    output.back().name = name;
}

size_t Parser::FindFunction(string name, const map<string, size_t>& FunctionList) const
{
    auto it = FunctionList.find(name);
    if (it == FunctionList.end())
    {
        fprintf(stderr, "could not find function with name %s\n", name.c_str());  //(position = %" PRIu32 ",%" PRIu32")\n", name.c_str(), name->GetLine(), name->GetColumn());
        return ((size_t) -1);
    }
    else
        return it->second;
}

size_t Parser::FindMetka(string name, const map<string, size_t>& MetkaList) const
{

    auto it = MetkaList.find(name);
    if (it == MetkaList.end())
    {
        fprintf(stderr, "could not find function with name %s\n", name.c_str());  //(position = %" PRIu32 ",%" PRIu32")\n", name->GetData().c_str(), name->GetLine(), name->GetColumn());
        return ((size_t) -1);
    }
    else
        return it->second;
}

vector<Function>& Parser::Parse()
{
    position = 0;
    while(position < length)
    {
        ReadFunction1();
        if (error)
            return output;
    }

    position = 0;
    while(position < length)
    {
        ReadFunction2();
        if (error)
            return output;
    }

    return output;
}

///on first read we should
///1)check function declaration
///2)constuct Funtion class if need (not)
///3)give it index number in output vector
///4)look through consts (check for correct syntax)
///5)look through code (check for correct syntax) and get metkas
size_t Parser::ReadFunction1()
{
    Match(TokenType::ID, "function", "expected function keyword");
    if (error)
        return 1;

    Compare(TokenType::ID, "expected function name (TokenType::ID)");
    if (error)
        return 2;

    AddFunction(Get(0)->GetData());
    Next();

    Match(TokenType::LParen, "expected '('");
    if (error)
        return 3;

    Match(TokenType::RParen, "expected ')'");
    if (error)
        return 4;

    Match(TokenType::Colon , "expected ':'");
    if (error)
        return 5;

    ReadConsts1();
    if (error)
        return 6;

    ReadCode1(&output.back());
    if (error)
        return 7;

    return 0;
}

///on second read we should
///1)read consts (carefully)
///2)read code   (carefully)
///3)generate Function class (ready for run)
size_t Parser::ReadFunction2()
{
    string FunctionName = Get(1)->GetData();
    size_t offset       = FindFunction(FunctionName, FunctionSearchList);
    if (offset == ((size_t) -1))
        return 1;
    //skip function declaration
    Skip(5);

    Function* dest = &output[offset];

    ReadConsts2(dest);


    ReadCode2(dest);
    if (error)
        return 2;
    return 0;
}

size_t Parser::ReadConst1()
{
    Match(TokenType::ID, "const", "expected const word");
    if (error)
        return 1;

    Match(TokenType::LBracket, "expected [");
    if (error)
        return 2;

    Match(TokenType::Number, "expected number");
    if (error)
        return 3;

    Match(TokenType::RBracket, "expected ]");
    if (error)
        return 4;

    Match(TokenType::Assign, "expected :=");
    if (error)
        return 5;

    Match(TokenType::Number, "expected number");
    if (error)
        return 6;

    return 0;
}

size_t Parser::ReadConst2(Function* dest)
{
    map<size_t, Object>* ConstList = &(dest->consts);

    size_t offset = 0;
    Token* temp   = Get(2);
    if (ParseSize(temp, offset) != temp->GetData().length())
    {
        error = 1;
        return 1;
    }


    int32_t value = 0;
            temp  = Get(5);
    if (ParseInt32(temp, value) != temp->GetData().length())
    {
        error = 1;
        return 2;
    }

    Object::set((*ConstList)[offset], value);
    Skip(6);
    return 0;
}

size_t Parser::ReadConsts1()
{
    size_t count = 0;
    while(!Match(TokenType::ID, "begin", nullptr))
    {
        ReadConst1();
        if (error)
            return count;
        ++count;
    }
    return 0;
}

size_t Parser::ReadConsts2(Function* dest)
{
    size_t count = 0;
    while(!Match(TokenType::ID, "begin", nullptr))
    {
        ReadConst2(dest);
        if (error)
            return count;
        ++count;
    }
    return 0;
}

bool Parser::IsMetka()
{
    Token* current = Get(0);
    Token* next    = Get(1);
    if (!current)
        return false;
    if (!next)
        return false;
    if (current->GetType() != TokenType::ID)
        return false;
    if (next   ->GetType() != TokenType::Colon)
        return false;
    return true;
}

size_t Parser::ReadMetka(size_t cmd_number, map<string, size_t>& MetkaList)
{
    assert(IsMetka());

    const string& MetkaName = Get(0)->GetData();

    auto it = MetkaList.find(MetkaName);
    if (it != MetkaList.end())
    {
        fprintf(stderr, "Multiple declaration of metka %s", MetkaName.c_str());
        return 2;
    }

    MetkaList[MetkaName] = cmd_number;
    Skip(2);
    return 0;
}

size_t Parser::ReadCode1(Function* dest)
{
    size_t count = 0;
    while(!Match(TokenType::ID, "end", nullptr))
    {
        if (IsMetka())
        {
            ReadMetka(count, dest->__MetkaList);
            continue;
        }
        if (ReadCommand1() != 0)
            return count;
        ++count;
    }

    dest->size = count;
    dest->Code = (Command*) calloc(dest->size, sizeof(Command));
    return 0;
}

size_t Parser::ReadCode2(Function* dest)
{
    size_t count = 0;
    while(!Match(TokenType::ID, "end", nullptr))
    {
        if (IsMetka())
        {
            Skip(2);
            continue;
        }
        if (ReadCommand2(dest->Code + count, dest->__MetkaList, FunctionSearchList) != 0)
            return count;
        ++count;
    }
    return 0;
}

size_t Parser::ReadCommand1()
{
    Token* current = Get(0);
    if (current->GetType() != TokenType::ID)
        return 1;

    string CommandName = current->GetData();
    //insert your command:
    if (CommandName == "push")
        return ReadPush(nullptr) != 5;
    if (CommandName == "pop")
        return ReadPop (nullptr) != 5;
    if (CommandName == "add")
    {
        Next();
        return 0;
    }
    if (CommandName == "ge")
    {
        Next();
        return 0;
    }
    if (CommandName == "jump")
    {
        Token* next = Get(1);
        if (next->GetType() != TokenType::ID)
        {
            fprintf(stderr, "expected metka name (TokenType::ID) after jump word (%d,%d)", next->line, next->column);
            return 3;
        }
        Skip(2);
        return 0;
    }
    if (CommandName == "ifjump")
    {
        Token* next = Get(1);
        if (next->GetType() != TokenType::ID)
        {
            fprintf(stderr, "expected metka name (TokenType::ID) after ifjump word (%d,%d)", next->line, next->column);
            return 3;
        }
        Skip(2);
        return 0;
    }
    if (CommandName == "call")
    {
        Token* next = Get(1);
        if (next->GetType() != TokenType::ID)
        {
            fprintf(stderr, "expected metka name (TokenType::ID) after call word (%d,%d)", next->line, next->column);
            return 3;
        }
        Skip(2);
        return 0;
    }

    return 2; //no command found
}

size_t Parser::ReadCommand2(Command* dest, const map<string, size_t>& MetkaList, const map<string, size_t>& FunctionList)
{
    string CommandName = Get(0)->GetData();
    if (CommandName == "push")
        return ReadPush(dest) != 5;
    if (CommandName == "pop")
        return ReadPop (dest) != 5;
    if (CommandName == "add")
    {
        dest->data[0] = 4;
        dest->size    = 1;
        Skip(1);
        return 0;
    }
    if (CommandName == "ge")
    {
        dest->data[0] = 0x15;
        dest->size    = 1;
        Skip(1);
        return 0;
    }
    if ((CommandName == "jump") || (CommandName == "ifjump"))
        return ReadJump(dest, MetkaList);
    if ((CommandName == "call"))
        return ReadCall(dest, FunctionList);
    return 1;
}

size_t Parser::ReadJump(Command* dest, const map<string, size_t>& MetkaList)
{
    assert(dest != nullptr);

    Token* current = Get(0);
    Token* next    = Get(1);

    assert((current->GetType() == TokenType::ID));
    assert((next   ->GetType() == TokenType::ID));
    assert((current->GetData() == "jump") || (current->GetData() == "ifjump"));

    size_t jump_type = 0;
    if (current->GetData() == "jump")
        jump_type = 1;
    if (current->GetData() == "ifjump")
        jump_type = 2;

    size_t offset = FindMetka(next->GetData(), MetkaList);
    if (offset == (size_t) -1)
        return 1;

    if (jump_type == 1)
        dest->data[0] = 0x20;
    if (jump_type == 2)
        dest->data[0] = 0x21;
    memcpy(dest->data + 1, &offset, sizeof(size_t));
    dest->size = 1 + sizeof(size_t);
    Skip(2);
    return 0;
}

size_t Parser::ReadCall(Command* dest, const map<string, size_t>& FunctionList)
{
    assert(dest != nullptr);

    Token* current = Get(0);
    Token* next    = Get(1);

    assert(current->GetType() == TokenType::ID);
    assert(next   ->GetType() == TokenType::ID);
    assert(current->GetData() == "call");

    size_t offset = FindFunction(next->GetData(), FunctionList);
    if (offset == (size_t) -1)
        return 1;

    dest->data[0] = 0x30;

    memcpy(dest->data + 1, &offset, sizeof(size_t));
    dest->size = 1 + sizeof(size_t);
    return 0;
}




/*
bool Parser::CommandRead(string name, size_t index, bool MetkaListReady)
{
    map<string, size_t>* MetkaList = &output.hidden[name].MetkaList;

    Token* current = Get(0);
    Token* next    = Get(1);

    //[metka] ?
    if ((current->GetType() == TokenType::ID) && (next->GetType() == TokenType::Colon))
    {
        (*MetkaList)[current->GetData()] = index;
        Skip(2);
    }

    if (Compare(TokenType::ID, "end"))
        return true;

    string CommandName = Get(0)->GetData();
    if (CommandName == "push")
        return ReadPush(&output[name].Code[index]) == 5;
    if (CommandName == "pop")
        return ReadPop (&output[name].Code[index]) == 5;
    if (CommandName == "add")
    {
        Command* dest = &output[name].Code[index];
        dest->data[0] = 4;
        dest->size    = 1;
        Next();
        return true;
    }
    if (CommandName == "ge")
    {
        Command* dest = &output[name].Code[index];
        dest->data[0] = 0x15;
        dest->size    = 1;
        Next();
        return true;
    }
    ///We use MetkaListReady because we can jump forward
    if ((CommandName == "jump") || (CommandName == "ifjump"))
    {
        if (MetkaListReady)
            return ReadJump(&output[name].Code[index], &output[name]) == 2;
        else
            return CommandCheck() == true;
    }
    ///For functions as in C we should declare function before use
    if (CommandName == "call")
        return ReadCall(&output[name].Code[index], &output) == 2;

    return false;
}
*/

size_t Parser::ReadPush(Command* dest)
{
    Token* current;

    current = Get(0);
    if (current->GetType() != TokenType::ID)
        return 0;
    if (current->GetData() != "push")
        return 0;

    current = Next();
    if ((current->GetType() != TokenType::ID))
    {
        fprintf(stderr, "Expected consts or memory keyword (position %" PRIu32 ",%" PRIu32")\n", current->GetLine(), current->GetColumn());
        return 1;
    }
    if ((current->GetData() != "consts") && (current->GetData() != "memory"))
    {
        fprintf(stderr, "Expected consts or memory keyword (position %" PRIu32 ",%" PRIu32")\n", current->GetLine(), current->GetColumn());
        return 1;
    }

    if (dest != nullptr)
    {
        if (current->GetData() == "consts")
        {
            dest->data[0] = 1;
        }
        if (current->GetData() == "memory")
        {
            dest->data[0] = 2;
        }
    }

    current = Next();
    if (current->GetType() != TokenType::LBracket)
    {
        fprintf(stderr, "Expected '[' (position %" PRIu32 ",%" PRIu32")\n", current->GetLine(), current->GetColumn());
        return 2;
    }

    current = Next();
    if (current->GetType() != TokenType::Number)
    {
        fprintf(stderr, "Expected TokenType::Number (position %" PRIu32 ",%" PRIu32")\n", current->GetLine(), current->GetColumn());
        return 3;
    }

    if (dest != nullptr)
    {
        size_t offset;
        ParseSize(current, offset);
        memcpy(dest->data + 1, &offset, sizeof(size_t));
    }

    current = Next();
    if (current->GetType() != TokenType::RBracket)
    {
        fprintf(stderr, "Expected ']' (position %" PRIu32 ",%" PRIu32")\n", current->GetLine(), current->GetColumn());
        return 4;
    }

    Next();
    if (dest != nullptr)
        dest->size = 1 + sizeof(size_t);
    return 5;
}

size_t Parser::ReadPop(Command* dest)
{
    Token* current;

    current = Get(0);
    if (current->GetType() != TokenType::ID)
        return 0;
    if (current->GetData() != "pop")
        return 0;

    current = Next();
    if (current->GetType() != TokenType::ID)
    {
        fprintf(stderr, "Expected memory keyword (position %" PRIu32 ",%" PRIu32")\n", current->GetLine(), current->GetColumn());
        return 1;
    }
    if (current->GetData() != "memory")
    {
        fprintf(stderr, "Expected memory keyword (position %" PRIu32 ",%" PRIu32")\n", current->GetLine(), current->GetColumn());
        return 1;
    }

    if (dest != nullptr)
    {
        if (current->GetData() == "memory")
        {
            dest->data[0] = 3;
        }
    }

    current = Next();
    if (current->GetType() != TokenType::LBracket)
    {
        fprintf(stderr, "Expected '[' (position %" PRIu32 ",%" PRIu32")\n", current->GetLine(), current->GetColumn());
        return 2;
    }

    current = Next();
    if (current->GetType() != TokenType::Number)
    {
        fprintf(stderr, "Expected TokenType::Number (position %" PRIu32 ",%" PRIu32")\n", current->GetLine(), current->GetColumn());
        return 3;
    }

    if (dest != nullptr)
    {
        size_t offset;
        ParseSize(current, offset);
        memcpy(dest->data + 1, &offset, sizeof(size_t));
    }

    current = Next();
    if (current->GetType() != TokenType::RBracket)
    {
        fprintf(stderr, "Expected ']' (position %" PRIu32 ",%" PRIu32")\n", current->GetLine(), current->GetColumn());
        return 4;
    }

    Next();
    if (dest != nullptr)
        dest->size = 1 + sizeof(size_t);
    return 5;
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
