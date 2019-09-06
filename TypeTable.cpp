#include "TypeTable.hpp"
#include <cstdio>
#include <cinttypes>

/*
void TypeTable::AddNewType(string name, size_t size)
{
    types.push_back(Type(name, size));
}

void TypeTable::AddNewType(string name, size_t size, FunctionClass* ctor, FunctionClass* dtor)
{
    types.push_back(Type(name, size, ctor, dtor));
}

const Type* TypeTable::GetTypeByName(string name)
{
    for(auto type : types)
        if (type.GetName() == name)
            return &type;
}
*/

const FunctionClass* TypeTable::find(string name, size_t argc, ArgumentType* argv)
{
    Functions.find(name);
    if (this->Functions.find(name) == this->Functions.end())
        return nullptr;

    const vector<FunctionClass>& FunctionVector = Functions[name];
    for(vector<FunctionClass>::const_iterator it = FunctionVector.begin(); it != FunctionVector.end(); ++it)
    {
#define function (*it)
        if (function.argc == argc)
        {
            size_t i;
            for(i = 0; i < argc; ++i)
            {
                if (function.argv[i].is_const != argv[i].is_const)
                    break;
                if (function.argv[i].type     != argv[i].type)
                    break;
            }
            if (i == argc)
                return &function;
        }
#undef function
    }
    return nullptr;
}

const FunctionClass* TypeTable::insert(string name, size_t argc, ArgumentType* argv, FCFunction func)
{
    FunctionClass temp;
    if (temp.init(argc, argv, func) == nullptr)
        return nullptr;
    //if this name does not exist, operator [] creates it
    Functions[name].push_back(temp);
    return find(name, argc, argv);
}

int TypeTable::call(string name, size_t argc, Argument* argv)
{
    ArgumentType arguments_type[argc];
    for(size_t i = 0; i < argc; ++i)
    {
        arguments_type[i].is_const = argv[i].is_const;
        arguments_type[i].type     = argv[i].argument->type;
    }

    const FunctionClass* fn = find(name, argc, arguments_type);
    if (fn == nullptr)
        return 1;

    return (*fn)(argc, argv);
}


void TypeTable::BuiltinInit()
{
    ///add assignment
    {
        ArgumentType argv[2];
        argv[0].is_const = false;
        argv[1].is_const = true;

        argv[0].type = ObjectType::Int32;
        argv[1].type = ObjectType::Int32;
        insert("add", 2, argv, TypeTable::Object_Builtin_Int32_Int32_add_assignment);

        argv[0].type = ObjectType::Int64;
        argv[1].type = ObjectType::Int64;
        insert("add", 2, argv, TypeTable::Object_Builtin_Int64_Int64_add_assignment);

        argv[0].type = ObjectType::Size;
        argv[1].type = ObjectType::Size;
        insert("add", 2, argv, TypeTable::Object_Builtin_Size_Size_add_assignment);
    }

    ///ge
    {
        ArgumentType argv[3];
        argv[0].is_const = false;
        argv[0].type     = ObjectType::Bool;
        argv[1].is_const = true;
        argv[2].is_const = true;

        argv[1].type = ObjectType::Int32;
        argv[2].type = ObjectType::Int32;
        insert("ge", 3, argv, TypeTable::Object_Builtin_Int32_Int32_ge);
    }

    ///to string
    {
        ArgumentType argv[2];
        argv[0].is_const = false;
        argv[0].type     = ObjectType::String255;
        argv[1].is_const = true;

        argv[1].type = ObjectType::Int32;
        insert("string", 2, argv, TypeTable::Object_Builtin_Int32_To_String);

        argv[1].type = ObjectType::Int64;
        insert("string", 2, argv, TypeTable::Object_Builtin_Int64_To_String);

        argv[1].type = ObjectType::Size;
        insert("string", 2, argv, TypeTable::Object_Builtin_Size_To_String);
    }
}

void TypeTable::BuiltinDestroy()
{
    for(auto vec_functions : Functions)
        for(auto function : vec_functions.second)
            function.destroy();
}

/**
    TO DO:
        Builtin methods should work with data stack instead of stand alone methods
        it is better for productivity and code correct (fucntion should clear it's arguments)

    Example of new function definition:
        function <name>(ObjectType arg1, ObjectType arg2, ...) -> ObjectType res1, ObjectType res2:
            ...
        begin
            ...
        end

    Data stack before and after call:
        ..., arg_1, arg_2, ..., arg_n -> ..., res_1, res_2, ..., res_m

    if require memory
        what to do??? (2 pairs of "()") or ( () and [] )


*/

///----------------
///Add Assignment
///----------------

///..., Int32, Int32 -> ..., Int32
void TypeTable::Object_Builtin_Int32_Int32_add_assignment(size_t argc, Argument* argv)
{
    assert(argc == 2);

          int32_t& left  = Object::get<int32_t>(*(argv[0].argument));
    const int32_t& right = Object::get<int32_t>(*(argv[1].argument));
    left += right;
}

void TypeTable::Object_Builtin_Int64_Int64_add_assignment(size_t argc, Argument* argv)
{
    assert(argc == 2);

          int64_t& left  = Object::get<int64_t>(*(argv[0].argument));
    const int64_t& right = Object::get<int64_t>(*(argv[1].argument));
    left += right;
}

void TypeTable::Object_Builtin_Size_Size_add_assignment(size_t argc, Argument* argv)
{
    assert(argc == 2);

          size_t& left  = Object::get<size_t>(*(argv[0].argument));
    const size_t& right = Object::get<size_t>(*(argv[1].argument));
    left += right;
}

///------
///GE
///------

void TypeTable::Object_Builtin_Int32_Int32_ge(size_t argc, Argument* argv)
{
    assert(argc == 3);


    //some pointers in argv can be equal so we need to do things accurate
    const int32_t& left  = Object::get<int32_t>(*(argv[1].argument));
    const int32_t& right = Object::get<int32_t>(*(argv[2].argument));

    Object::set(*(argv[0].argument), left >= right);
}

///-----------
///Inc
///-----------

void TypeTable::Object_Builtin_Int32_inc(size_t argc, Argument* argv)
{
    assert(argc == 1);

    ++Object::get<int32_t>(*(argv[0].argument));
}

///----------
///To String
///----------

void TypeTable::Object_Builtin_Size_To_String(size_t argc, Argument* argv)
{
    assert(argc == 2);

    void* data = argv[0].argument->data;

    *(int8_t*) data = sprintf((char*) data + 1, "%" PRIu32, *(size_t*) argv[1].argument->data);
}

void TypeTable::Object_Builtin_Int32_To_String(size_t argc, Argument* argv)
{
    assert(argc == 2);

    void* data = argv[0].argument->data;

    *(int8_t*) data = sprintf((char*) data + 1, "%" PRId32, *(int32_t*) argv[1].argument->data);
}

void TypeTable::Object_Builtin_Int64_To_String(size_t argc, Argument* argv)
{
    assert(argc == 2);

    void* data = argv[0].argument->data;

    *(int8_t*) data = sprintf((char*) data + 1, "%" PRId64, *(int64_t*) argv[1].argument->data);
}
