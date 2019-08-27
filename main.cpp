#include <iostream>
#include <fstream>
#include <cassert>


#include "Lexer.hpp"
#include "Parser.hpp"

struct RunState
{
    Function* fn; //function number in functions list
    size_t    position;
};

struct CallStack
{
    RunState* start;
    RunState* finish;
    RunState* current;
};

struct DataStack
{
    Object* start;
    Object* finish;
    Object* current;
};

struct ExceptionStack
{
    void* start;
    void* finish;
    void* current;
};

struct state
{
    ExceptionStack estack;
    CallStack      cstack;
    DataStack      dstack;

    Object memory[256];

    Function* FunctionList;
    size_t    size;
    Function* fn;    //function number
    size_t    position;
};

bool init(state* st, std::vector<Function>* functions)
{
    assert(st        != nullptr);
    assert(functions != nullptr);

    st->size         = functions->size();
    st->FunctionList = functions->data();

    DataStack* dstack = &(st->dstack);
    dstack->start     = (Object*) calloc(256, sizeof(Object));
    if (dstack->start == nullptr)
        return false;
    dstack->finish    = dstack->start + 256;
    dstack->current   = dstack->start;

    CallStack* cstack = &(st->cstack);
    cstack->start     = (RunState*) calloc(256, sizeof(RunState));
    if (cstack->start == nullptr)
        return false;
    cstack->finish    = cstack->start + 256;
    cstack->current   = cstack->start;

    st->fn       = nullptr;
    st->position = 0;
    for(size_t i = 0; i < st->size; ++i)
        if (st->FunctionList[i].name == "main")
        {
            st->fn = &st->FunctionList[i];
            break;
        }
    if (st->fn == nullptr)
        return false;

    return true;
}

void push(state* st, size_t argc, Object* argv)
{
    if (argc == 0)
        return;
    for(size_t i = 0; i < argc; ++i)
    {
        Object::copy(st->dstack.current[i], argv[i]);
    }
    st->dstack.current += argc;
}

void pop (state* st, size_t argc, Object* argv)
{
    if (argc == 0)
        return;
    if (argv == nullptr)
    {
        ///works as clear
        st->dstack.current -= argc;
        for(size_t i = 0; i < argc; ++i)
            st->dstack.current[i].clear();
    }
    else
    {
        st->dstack.current -= argc;
        for(size_t i = 0; i < argc; ++i)
        {
            Object::move(argv[i], st->dstack.current[i]);
        }
    }
}

void add(state* st)
{
    ObjectType ltype, rtype;
    ltype = st->dstack.current[-2].GetType();
    rtype = st->dstack.current[-1].GetType();

    if ((ltype == ObjectType::Int32) && (rtype == ObjectType::Int32))
    {
        Object::get<int32_t>(st->dstack.current[-2]) += Object::get<int32_t>(st->dstack.current[-1]);
    }
}

///@arg 1 (size_t) - offset in consts list
int push_const(Command cmd, state* st)
{
    size_t offset;
    memcpy(&offset, cmd.data + 1, sizeof(offset));

    push(st, 1, &st->fn->consts[offset]);
    return 0;
}

///@arg 1 (size_t) - offset in memory
int load_memory(Command cmd, state* st)
{
    size_t offset;
    memcpy(&offset, cmd.data + 1, sizeof(offset));

    push(st, 1, st->memory + offset);

    return 0;
}

///@arg 1 (size_t) - offset in memory
int store_memory(Command cmd, state* st)
{
    size_t offset;
    memcpy(&offset, cmd.data + 1, sizeof(offset));

    pop (st, 1, st->memory + offset);

    return 0;
}

///@arg 1(dstack[-2]) - left  operand
///@arg 2(dstack[-1]) - right operand
int add(Command cmd, state* st)
{
    ObjectType ltype = st->dstack.current[-2].GetType();
    ObjectType rtype = st->dstack.current[-1].GetType();

    operator2 add_fn = Object::add2_function(ltype, rtype);
    add_fn(st->dstack.current[-2], st->dstack.current[-1]);

    pop(st, 1, nullptr);

    return 0;
}

int eq(Command cmd, state* st)
{
    ObjectType ltype = st->dstack.current[-2].GetType();
    ObjectType rtype = st->dstack.current[-1].GetType();

    operator3 eq_fn = Object::eq_function(ltype, rtype);
    Object result = eq_fn(st->dstack.current[-2], st->dstack.current[-1]);

    pop (st, 2, nullptr);
    push(st, 1, &result);
    //st->dstack.current[-1].clear();
    //st->dstack.current[-2].clear();
    //Object::move(st->dstack.current[-2], result);
    //--st->dstack.current;

    return 0;
}

int ne(Command cmd, state* st)
{
    ObjectType ltype = st->dstack.current[-2].GetType();
    ObjectType rtype = st->dstack.current[-1].GetType();

    operator3 ne_fn = Object::ne_function(ltype, rtype);
    Object result = ne_fn(st->dstack.current[-2], st->dstack.current[-1]);

    pop (st, 2, nullptr);
    push(st, 1, &result);
    //st->dstack.current[-1].clear();
    //st->dstack.current[-2].clear();
    //Object::move(st->dstack.current[-2], result);
    //--st->dstack.current;

    return 0;
}

int lt(Command cmd, state* st)
{
    ObjectType ltype = st->dstack.current[-2].GetType();
    ObjectType rtype = st->dstack.current[-1].GetType();

    operator3 lt_fn = Object::lt_function(ltype, rtype);
    Object result = lt_fn(st->dstack.current[-2], st->dstack.current[-1]);

    pop (st, 2, nullptr);
    push(st, 1, &result);
    //st->dstack.current[-1].clear();
    //st->dstack.current[-2].clear();
    //Object::move(st->dstack.current[-2], result);
    //--st->dstack.current;

    return 0;
}

int le(Command cmd, state* st)
{
    ObjectType ltype = st->dstack.current[-2].GetType();
    ObjectType rtype = st->dstack.current[-1].GetType();

    operator3 le_fn = Object::le_function(ltype, rtype);
    Object result = le_fn(st->dstack.current[-2], st->dstack.current[-1]);

    pop (st, 2, nullptr);
    push(st, 1, &result);
    //st->dstack.current[-1].clear();
    //st->dstack.current[-2].clear();
    //Object::move(st->dstack.current[-2], result);
    //--st->dstack.current;

    return 0;
}

int gt(Command cmd, state* st)
{
    ObjectType ltype = st->dstack.current[-2].GetType();
    ObjectType rtype = st->dstack.current[-1].GetType();

    operator3 gt_fn = Object::gt_function(ltype, rtype);
    Object result = gt_fn(st->dstack.current[-2], st->dstack.current[-1]);

    pop (st, 2, nullptr);
    push(st, 1, &result);
    //st->dstack.current[-1].clear();
    //st->dstack.current[-2].clear();
    //Object::move(st->dstack.current[-2], result);
    //--st->dstack.current;

    return 0;
}

int ge(Command cmd, state* st)
{
    ObjectType ltype = st->dstack.current[-2].GetType();
    ObjectType rtype = st->dstack.current[-1].GetType();

    operator3 ge_fn = Object::ge_function(ltype, rtype);
    Object result = ge_fn(st->dstack.current[-2], st->dstack.current[-1]);

    pop (st, 2, nullptr);
    push(st, 1, &result);
    //st->dstack.current[-1].clear();
    //st->dstack.current[-2].clear();
    //Object::move(st->dstack.current[-2], result);
    //--st->dstack.current;

    return 0;
}

/*
int jump(Command cmd, state* st)
{
    int8_t offset =  dereference<int8_t>(code->current + sizeof(cmd_type));

    code->current += offset;
    return 0;
}

int jump_if(Command cmd, state* st)
{
    if ((size_t) (st->dstack.current - st->dstack.start) == 0)
    {
        throw "Instr Error: not enough arguments for call jump\n";
    }

    Object temp;
    pop(st, 1, &temp);

    if (Object::get<bool>(temp))
    {
        jump(code, st);
        return 0;
    }
    else
    {
        code->current += sizeof(cmd_type) * 2;
        return 0;
    }
}
*/

/**
    command - decoded ptr
    state   - state
    next    - data for command (command + sizeof(cmd_type))
    @return next instruction
*/
int run_one(state* st)
{
    const Command* code = st->fn->Code;
    const Command& cmd  = code[st->position];
    switch(cmd.data[0])
    {
        case 0:
            ++st->position;
            return 0;
        case 1:
            return push_const  (code[st->position++], st);
        case 2:
            return load_memory (code[st->position++], st);
        case 3:
            return store_memory(code[st->position++], st);
        case 4:
            return add(code[st->position++], st);
        case 0x10:
            return eq(code[st->position++], st);
        case 0x11:
            return ne(code[st->position++], st);
        case 0x12:
            return lt(code[st->position++], st);
        case 0x13:
            return le(code[st->position++], st);
        case 0x14:
            return gt(code[st->position++], st);
        case 0x15:
            return ge(code[st->position++], st);
        case 0x20:
        {
            memcpy(&st->position, cmd.data + 1, sizeof(size_t));
            return 0;
        }
        case 0x21:
        {
            if ((size_t) (st->dstack.current - st->dstack.start) == 0)
            {
                throw "Instr Error: not enough arguments for call jump\n";
            }

            Object temp;
            pop(st, 1, &temp);

            if (Object::get<bool>(temp))
            {
                memcpy(&st->position, cmd.data + 1, sizeof(size_t));
                return 0;
            }
            else
            {
                ++st->position;
                return 0;
            }
        }
        //call function
        case 0x30:
        {
            CallStack cstack = st->cstack;
            cstack.current[0].fn       = st->fn;
            cstack.current[0].position = st->position;
            ++cstack.current;

            size_t offset;
            memcpy(&offset, cmd.data + 1, sizeof(size_t));
            if (offset >= st->size)
            {
                fprintf(stderr, "index of function in command call is out of range");
                return 1;
            }

            st->fn       = &st->FunctionList[offset];
            st->position = 0;
        }
        default:
            return -1;
    }
}


int run(state* st)
{
    int    count    = 0;
    int    error    = 0;
    while(true)
    {
        if ((0 <= st->position) && (st->position < st->fn->size))
        {
            error = run_one(st);
            if (error)
            {
                fprintf(stderr, "error in run_one() = %d, iteration = %d", error, count);
                break;
            }
        }
        else
        {
            fprintf(stderr, "code borders corruption");
            return 1;
        }
        ++count;
    }
    return 0;
}

std::string ReadFile(const char* name)
{
    std::ifstream    fin;
    fin.open(name);
  
    if (!fin)
    {
        std::cout << "file not opened";
        return "";
    }

    std::string temp = "";
    std::string text = "";

    while(getline(fin, temp))
        text += temp + '\n';

    return text;
}

int main()
{
    std::string text = ReadFile("code.txt");

    Lexer lex(text);
    std::vector<Token> tokens = lex.Tokenize();
    for(auto token : tokens)
        printf("%s\n", ToString(token).c_str());

    Parser par(tokens);
    std::vector<Function>& functions = par.Parse();

    for(std::vector<Function>::iterator it = functions.begin(); it != functions.end(); ++it)
    {
        fprintf(stdout, "function %s():\n", it->name.c_str());
        for(auto Const : it->consts)
        {
            fprintf(stdout, "%2x ", Const.first);
            fprintf(stdout, "%p\n", Const.second.data);
        }
        for(size_t i = 0; i < it->size; ++i)
        {
            Command* temp = it->Code + i;
            for(size_t j = 0; j < temp->size; ++j)
            {
                fprintf(stdout, "%2x ", temp->data[j]);
            }
            fprintf(stdout, "\n");
        }
    }



    state st;
    if (!init(&st, &functions))
        return 1;

    run(&st);
    return 0;
}
