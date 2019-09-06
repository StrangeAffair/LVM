#ifndef _TypeTable_hpp_
#define _TypeTable_hpp_ 1

#include "Object.hpp"
#include "FunctionClass.hpp"

#include <cassert>

#include <initializer_list>
#include <string>
#include <vector>
#include <map>

using std::initializer_list;
using std::string;
using std::vector;
using std::map;

class TypeTable;

class Type
{
    string name;
    size_t size; //basic size
    map<string, vector<FunctionClass*>> methods;

public:
    Type(string name, size_t size):
        name(name), size(size)
    {}
    Type(string name, size_t size, FunctionClass* ctor, FunctionClass* dtor):
        Type(name, size)
    {
        methods["ctor"].push_back(ctor);
        methods["dtor"].push_back(dtor);
    }
public:
    const string& GetName() const
    {return name;}
    const size_t& GetSize() const
    {return size;}
public:
    const FunctionClass* find  (string name, size_t argc, Argument* argv);
    const FunctionClass* insert(string name, FunctionClass* method);
};

class TypeTable
{
//    vector<Type> types;
    map<string, vector<FunctionClass>> Functions;
private: //should be public
    const FunctionClass* find  (string name, size_t argc, ArgumentType* argv);
public:
    const FunctionClass* insert(string name, size_t argc, ArgumentType* argv, FCFunction func);
public:
    //inits and destroys builtin functions
    void BuiltinInit   ();
    void BuiltinDestroy();
public:
    int  call(string name, size_t argc, Argument* argv);
    //int  call(string name, size_t argc, Object** argv, bool* is_const);
    //int  call(string name, size_t argc, initializer_list<Object*> argv, initializer_list<bool> is_const);

/*
public:
    void AddNewType(string name, size_t size);
    void AddNewType(string name, size_t size, FunctionClass* ctor, FunctionClass* dtor);
public:
    const Type* GetTypeByName(string name);
*/
///Object Builtins
protected:
    static void Object_Builtin_Size_To_String(size_t argc, Argument* argv);
    static void Object_Builtin_Int32_To_String(size_t argc, Argument* argv);
    static void Object_Builtin_Int64_To_String(size_t argc, Argument* argv);

    static void Object_Builtin_Int32_Int32_ge(size_t argc, Argument* argv);

    static void Object_Builtin_Int32_inc(size_t argc, Argument* argv);

    static void Object_Builtin_Int32_Int32_add_assignment(size_t argc, Argument* argv);
    static void Object_Builtin_Int64_Int64_add_assignment(size_t argc, Argument* argv);
    static void Object_Builtin_Size_Size_add_assignment(size_t argc, Argument* argv);
};

#endif // _TypeTable_hpp_
