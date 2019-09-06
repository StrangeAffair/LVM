#include "Runner.hpp"

bool cinit(CallStack* st, size_t size)
{
    st->start   = (RunState*) calloc(size, sizeof(RunState));
    if (st->start == nullptr)
        return false;
    st->finish  = st->start + size;
    st->current = st->start;
    return true;
}

bool cfree(CallStack* st)
{
    if (st->start != nullptr)
        free(st->start);
    st->start   = nullptr;
    st->finish  = nullptr;
    st->current = nullptr;
    return true;
}

bool cpush(CallStack* st, RunState fn)
{
    if (st->current == st->finish)
        return false;
    st->current->fn       = fn.fn;
    st->current->position = fn.position;
    ++st->current;
    return true;
}

bool cpop(CallStack* st)
{
    if (st->current == st->start)
        return false;
    --st->current;
    return true;
}

size_t csize(CallStack* st)
{
    return (size_t) (st->current - st->start);
}

bool dinit(DataStack* st, size_t size)
{
    st->start   = (Object*) calloc(size, sizeof(Object));
    if (st->start == nullptr)
        return false;
    st->finish  = st->start + size;
    st->current = st->start;
    return true;
}

bool dfree(DataStack* st)
{
    if (st->start != nullptr)
        free(st->start);
    st->start   = nullptr;
    st->finish  = nullptr;
    st->current = nullptr;
    return true;
}

bool dpush(DataStack* st, size_t argc, Object* argv)
{
    assert(argv != nullptr);

    for(size_t i = 0; i < argc; ++i)
    {
        Object::copy(st->current[i], argv[i]);
    }
    st->current += argc;
    return true;
}

bool dpop (DataStack* st, size_t argc, Object* argv)
{
    if (argv == nullptr)
    {
        ///works as clear
        st->current -= argc;
        for(size_t i = 0; i < argc; ++i)
            Object::clear(st->current[i]);
    }
    else
    {
        st->current -= argc;
        for(size_t i = 0; i < argc; ++i)
        {
            Object::move (argv[i], st->current[i]);
        }
    }
    return true;
}

size_t dsize(DataStack* st)
{
    return (size_t) (st->current - st->start);
}

bool init(state* st, std::vector<Function>* functions)
{
    assert(st        != nullptr);
    assert(functions != nullptr);

    st->size         = functions->size();
    st->FunctionList = functions->data();

    if (!dinit(&st->dstack, 256))
        return false;
    if (!cinit(&st->cstack, 256))
        return false;

    for(size_t i = 0; i < st->size; ++i)
        if (st->FunctionList[i].name == "main")
        {
            RunState temp;
            temp.fn       = i;
            temp.position = 0;
            cpush(&st->cstack, temp);
            break;
        }
    if (csize(&st->cstack) == 0)
        return false;

    st->types.BuiltinInit();
    return true;
}

bool free(state* st)
{
    cfree(&st->cstack);
    dfree(&st->dstack);
    return true;
}

///@arg 1 (size_t) - offset in consts list
int push_const(Command cmd, state* st)
{
    RunState* curr = &st->cstack.current[-1];
    Function* fn   = &st->FunctionList[curr->fn];

    size_t offset;
    memcpy(&offset, cmd.data + 1, sizeof(offset));

    dpush(&st->dstack, 1, &fn->consts[offset]);
    ++curr->position;
    return 0;
}

///@arg 1 (size_t) - offset in memory
int load_memory(Command cmd, state* st)
{
    RunState* curr = &st->cstack.current[-1];

    size_t offset;
    memcpy(&offset, cmd.data + 1, sizeof(offset));

    dpush(&st->dstack, 1, st->memory + offset);
    ++curr->position;
    return 0;
}

///@arg 1 (size_t) - offset in memory
int store_memory(Command cmd, state* st)
{
    RunState* curr = &st->cstack.current[-1];

    size_t offset;
    memcpy(&offset, cmd.data + 1, sizeof(offset));

    dpop (&st->dstack, 1, st->memory + offset);
    ++curr->position;
    return 0;
}

size_t& RunnerPosition(state* st)
{
    RunState* curr = st->cstack.current - 1;
    return curr->position;
}

/*
///@arg 1(dstack[-2]) - left  operand
///@arg 2(dstack[-1]) - right operand
int add(Command cmd, state* st)
{
    //ObjectType ltype = st->dstack.current[-2].GetType();
    //ObjectType rtype = st->dstack.current[-1].GetType();


    RunState* curr = &st->cstack.current[-1];

    TypeClass argv[2];
    argv[0].is_const = false;
    argv[0].type     = st->dstack.current[-2].GetType();
    argv[1].is_const = true;
    argv[1].type     = st->dstack.current[-1].GetType();

    Object* args[2];
    args[0] = &st->dstack.current[-2];
    args[1] = &st->dstack.current[-1];

    const FunctionClass* add_fn = st->types.find("add", 2, argv);
    (*add_fn)(2, args);

    dpop(&st->dstack, 1, nullptr);
    ++curr->position;
    return 0;
}
*/

/*
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
*/

/*
int ge(Command cmd, state* st)
{

    //ObjectType ltype = st->dstack.current[-2].GetType();
    //ObjectType rtype = st->dstack.current[-1].GetType();

    RunState* curr = &st->cstack.current[-1];

    TypeClass argv[3];
    argv[0].is_const = false;
    argv[0].type     = ObjectType::Bool;
    argv[1].is_const = true;
    argv[1].type     = st->dstack.current[-2].GetType();
    argv[2].is_const = true;
    argv[2].type     = st->dstack.current[-1].GetType();

    Object result;

    Object* args[3];
    args[0] = &result;
    args[0] = &st->dstack.current[-2];
    args[1] = &st->dstack.current[-1];

    const FunctionClass* ge_fn = st->types.find("ge", 3, argv);
    (*ge_fn)(3, args);

    dpop (&st->dstack, 2, nullptr);
    dpush(&st->dstack, 1, &result);
    ++curr->position;
    return 0;
}
*/


int jge(Command cmd, state* st)
{
    /*
    ObjectType ltype = st->dstack.current[-2].GetType();
    ObjectType rtype = st->dstack.current[-1].GetType();
    */

    Object result;
    result.SetType(ObjectType::Bool);

    Argument args[3];
    args[0].is_const = false;
    args[0].argument = &result;
    args[1].is_const = true;
    args[1].argument = st->dstack.current - 2;
    args[2].is_const = true;
    args[2].argument = st->dstack.current - 1;

    int error = st->types.call("ge", 3, args);
    if (error)
        return error;

    dpop(&st->dstack, 2, nullptr);


    RunState* curr = st->cstack.current - 1;
    if (Object::get<bool>(result))
    {
        memcpy(&curr->position, cmd.data + 1, sizeof(size_t));
        return 0;
    }
    else
    {
        ++curr->position;
        return 0;
    }
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
    RunState* curr = &st->cstack.current[-1];

    const Function* fn   = &st->FunctionList[curr->fn];
    const Command*  code = fn->Code;
    const Command&  cmd  = code[curr->position];
    switch(cmd.data[0])
    {
        case 0:
            ++curr->position;
            return 0;
        case 1:
            return push_const  (code[curr->position], st);
        case 2:
            return load_memory (code[curr->position], st);
        case 3:
            return store_memory(code[curr->position], st);
        case 4:
        {
            Argument args[2];
            args[0].is_const = false;
            args[0].argument = st->dstack.current - 2;
            args[1].is_const = true;
            args[1].argument = st->dstack.current - 1;

            int error = st->types.call("add", 2, args);
            if (error)
                return error;

            dpop(&st->dstack, 1, nullptr);
            ++RunnerPosition(st);
            return 0;
        }
        /*
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
        */
        case 0x15:
        {
            Object temp;
            temp.SetType(ObjectType::Bool);

            Argument args[3];
            args[0].is_const = false;
            args[0].argument = &temp;
            args[0].is_const = true;
            args[0].argument = st->dstack.current - 2;
            args[1].is_const = true;
            args[1].argument = st->dstack.current - 1;

            int error = st->types.call("ge", 3, args);
            if (error)
                return error;

            dpop (&st->dstack, 2, nullptr);
            dpush(&st->dstack, 1, &temp);
            ++RunnerPosition(st);
            return 0;
        }
        //jump
        case 0x20:
        {
            memcpy(&curr->position, cmd.data + 1, sizeof(size_t));
            return 0;
        }
        /*
        case 0x21:
            jeq
        case 0x22:
            jne
        case 0x23:
            jlt
        case 0x24:
            jle
        case 0x25:
            jgt
        */
        case 0x26:
            return jge(code[curr->position], st);
        //call function
        case 0x30:
        {
            size_t offset;
            memcpy(&offset, cmd.data + 1, sizeof(size_t));
            if (offset >= st->size)
            {
                fprintf(stderr, "index of function in command call is out of range");
                return 1;
            }

            ++RunnerPosition(st);

            RunState temp;
            temp.fn       = offset;
            temp.position = 0;

            cpush(&st->cstack, temp);
            return 0;
        }
        case 0x31:
        {
            cpop(&st->cstack);
            return 0;
        }
        case 0x40:
        {
            Argument args[1];
            args[0].is_const = false;
            args[0].argument = st->dstack.current - 1;

            return st->types.call("inc", 1, args);
        }
        default:
            return -1;
    }
}

void RunnerPrint(state* st);

int run(std::vector<Function>* functions)
{
    state st;
    if (!init(&st, functions))
        return 3;

    int count = 0;
    int error = 0;
    while(true)
    {
        if (csize(&st.cstack) == 0)
            break;

        size_t position     = st.cstack.current[-1].position;
        size_t function_end = st.FunctionList[st.cstack.current[-1].fn].size;

        if (position == function_end)
            for(RunState* it = (st.cstack.current - 1); it >= st.cstack.start; --it)
            {
                position     = st.cstack.current[-1].position;
                function_end = st.FunctionList[st.cstack.current[-1].fn].size;

                if (position == function_end)
                    cpop(&st.cstack);
                else
                    break;
            }

        if (csize(&st.cstack) == 0)
            break;

        if ((0 <= position) && (position < function_end))
        {
            error = run_one(&st);
            if (error)
            {
                fprintf(stderr, "error in run_one() = %d, iteration = %d", error, count);
                free(&st);
                return 2;
            }
            RunnerPrint(&st);
        }
        else
        {
            fprintf(stderr, "code borders corruption, iteration = %d", count);
            free(&st);
            return 2;
        }
        ++count;
    }
    RunnerPrint(&st);
    free(&st);
    return 0;
}

void RunnerPrint(state* st)
{
    fprintf(stdout, "----------\n");

    fprintf(stdout, "CStack:\n");
    for(RunState* it = st->cstack.start; it != st->cstack.current; ++it)
        fprintf(stdout, "\tfunction %2d, line %2d\n", it->fn, it->position);

    fprintf(stdout, "DStack:\n");
    for(Object* it = st->dstack.start; it != st->dstack.current; ++it)
        fprintf(stdout, "\tdata = %d\n", *(size_t*) it->data);

    fprintf(stdout, "Memory:\n");
    for(Object* it = st->memory; it != st->memory + 256; ++it)
        if (it->type != ObjectType::None)
            fprintf(stdout, "\tObject number %d = %d\n", it - st->memory, *(size_t*) it->data);

    fprintf(stdout, "----------\n");
}


