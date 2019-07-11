#include <iostream>
#include <fstream>

#include "Lexer.hpp"
#include "Parser.hpp"


struct DataStack
{
    Object* start;
    Object* finish;
    Object* current;
};

struct state
{
    DataStack dstack;

    Object consts[256];
    Object memory[256];
};


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

    push(st, 1, st->consts + offset);

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
int run_one(const std::vector<Command>& code, size_t& position, state* st)
{
    uint8_t instruction = code[position].data[0];
    switch(instruction)
    {
        case 0:
            ++position;
            return 0;
        case 1:
            return push_const (code[position++], st);
        case 2:
            return load_memory(code[position++], st);
        case 3:
            return store_memory(code[position++], st);
        case 4:
            return add(code[position++], st);
        case 0x10:
            return eq(code[position++], st);
        case 0x11:
            return ne(code[position++], st);
        case 0x12:
            return lt(code[position++], st);
        case 0x13:
            return le(code[position++], st);
        case 0x14:
            return gt(code[position++], st);
        case 0x15:
            return ge(code[position++], st);
        case 0x20:
        {
            Command temp = code[position];
            memcpy(&position, temp.data + 1, sizeof(size_t));
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
                Command temp = code[position];
                memcpy(&position, temp.data + 1, sizeof(size_t));
                return 0;
            }
            else
            {
                ++position;
                return 0;
            }
        }
        default:
            return -1;
    }
}

int run(const std::vector<Command>& code, state* st)
{
    int    count    = 0;
    int    error    = 0;
    size_t position = 0;
    while(true)
    {
        if ((0 <= position) && (position < code.size()))
        {
            error = run_one(code, position, st);
            if (error)
            {
                fprintf(stderr, "error in run_one() = %d, iteration = %d", error, count);
                break;
            }
        }
        else
            break;
        ++count;
    }
    if (position == code.size())
        return 0;
    else
    {
        fprintf(stderr, "code borders corruption");
        return 1;
    }
}


int main()
{
    std::ifstream    fin;
    fin.open("code.txt");

    if (!fin)
    {
        std::cout << "file not opened";
        return 1;
    }

    std::string temp = "";
    std::string text = "";

    while(getline(fin, temp))
        text += temp + '\n';

    Lexer lex(text);
    std::vector<Token> tokens = lex.Tokenize();
    for(auto token : tokens)
        printf("%s\n", ToString(token).c_str());

    Parser par(tokens);
    std::vector<Function> functions = par.Parse();

    state st;
    st.dstack.start   = (Object*) calloc(256, sizeof(Object));
    st.dstack.finish  = st.dstack.start + 256;
    st.dstack.current = st.dstack.start;
    std::map<size_t, Object> consts = functions[0].consts;
    for(auto Const : consts)
    {

        fprintf(stdout, "found i = %d\n", Const.first);
        Object::copy(st.consts[Const.first], consts[Const.first]);
        //if (consts.find(i) != consts.end())
        //{
        //    Object::copy(st.consts[i], consts[i]);
        //}
    }

    for(size_t i = 0; i < 5; ++i)
    {
        fprintf(stdout, "st->consts[%d] = %d\n", i, *(int32_t*) st.consts[i].data);
    }

    run(functions[0].code, &st);
    return 0;
}
