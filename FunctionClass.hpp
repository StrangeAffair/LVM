#ifndef _FunctionClass_hpp_
#define _FunctionClass_hpp_ 1

#include "Object.hpp"

/**
    stack->
        Object 1
        Object 2
        ...

    TypeTable.find("<func_name>", num_arg, ...[argTypes]) -> FunctionObject

    FunctionObject
    {
        size_t num_args;
        (func_ptr) void <name>(size_t argc, Object** argv);
        Type* types;
    }

    Type
    {
        bool       is_const;
        ObjectType type;
    }

    const size_t = {1, size_t};
*/

struct ArgumentType
{
    bool       is_const;
    ObjectType type;
};

struct Argument
{
    bool    is_const;
    Object* argument;
};

typedef void (*FCFunction)(size_t argc, Argument* argv);

class FunctionClass
{
public:
    size_t        argc;
    ArgumentType* argv;
    FCFunction    func;
public:
    FunctionClass* init   (size_t argc, ArgumentType* argv, FCFunction func);
    FunctionClass* destroy();
public:
    int operator()(size_t argc, Argument* argv) const;
};

#endif // _FunctionClass_hpp_
