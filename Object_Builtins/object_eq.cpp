#include "../Object.hpp"

static Object Builtin_Int32_Int32_eq(const Object& oleft, const Object& oright)
{
    Object oresult;
    oresult.SetType(ObjectType::Bool);

    bool&          result = Object::get<bool>   (oresult);
    const int32_t& left   = Object::get<int32_t>(oleft);
    const int32_t& right  = Object::get<int32_t>(oright);
    result = (left == right);
    return oresult;
}

static Object Builtin_Bool_Bool_eq(const Object& oleft, const Object& oright)
{
    Object oresult;
    oresult.SetType(ObjectType::Bool);

    bool&       result = Object::get<bool>(oresult);
    const bool& left   = Object::get<bool>(oleft);
    const bool& right  = Object::get<bool>(oright);
    result = (left == right);
    return oresult;
}

operator3 Object::eq_function(ObjectType left, ObjectType right)
{
    if (left == ObjectType::Int32 && right == ObjectType::Int32)
        return Builtin_Int32_Int32_eq;
    if (left == ObjectType::Bool  && right == ObjectType::Bool)
        return Builtin_Bool_Bool_eq;
    return nullptr;
}

