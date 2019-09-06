#include "Lexer.hpp"
#include "Parser.hpp"
#include "TypeTable.hpp"

struct RunState
{
    size_t fn; //function number in functions list
    size_t position;
};

struct CallStack
{
    RunState* start;
    RunState* finish;
    RunState* current;
};

static bool cinit(CallStack*, size_t size);
static bool cfree(CallStack*);
static bool cpush(CallStack*, RunState);
static bool cpop (CallStack*);

static size_t csize(CallStack*);

struct DataStack
{
    Object* start;
    Object* finish;
    Object* current;
};

static bool dinit(DataStack*, size_t size);
static bool dfree(DataStack*);
static bool dpush(DataStack*, size_t argc, Object* argv);
static bool dpop (DataStack*, size_t argc, Object* argv);

static size_t dsize(DataStack*);

/*
struct ExceptionStack
{
    void* start;
    void* finish;
    void* current;
};
*/

struct state
{
    /*
    ExceptionStack estack;
    */
    CallStack      cstack;
    DataStack      dstack;

    Object memory[256];

    Function* FunctionList;
    size_t    size;

    TypeTable types;
};

static bool init(state* st, std::vector<Function>* functions);
static bool free(state* st);

static int  push_const  (Command cmd, state* st);
static int  load_memory (Command cmd, state* st);
static int  store_memory(Command cmd, state* st);

static int  add(Command cmd, state* st);

static int   ge(Command cmd, state* st);
static int  jge(Command cmd, state* st);

static int run_one(state* st);
int run(std::vector<Function>* functions);
