#include "FunctionClass.hpp"

FunctionClass* FunctionClass::init(size_t argc, ArgumentType* argv, FCFunction func)
{
    this->argc = argc;
    this->argv = (ArgumentType*) calloc(argc, sizeof(ArgumentType));
    if (this->argv == nullptr)
        return nullptr;
    for(size_t i = 0; i < argc; ++i)
        this->argv[i] = argv[i];
    this->func = func;
    return this;
}

FunctionClass* FunctionClass::destroy()
{
    ///if error in "destructor" you should return this
    ///else return nullptr
    ///(errors could be added later)
    free(this->argv);
    this->argc = 0;
    this->argv = 0;
    this->func = nullptr;
    return nullptr;
}

int FunctionClass::operator()(size_t argc, Argument* argv) const
{
    if (this->argc != argc)
        return -1;
    for(size_t i = 0; i < argc; ++i)
        if (this->argv[i].type != argv[i].argument->type)
            return i + 1;
    //do call
    func(argc, argv);
    //all ok
    return 0;
    ///if function can return error it should return Exception type in its return value
}
