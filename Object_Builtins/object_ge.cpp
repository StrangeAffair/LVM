#include "../Object.hpp"

static Object Builtin_Int32_Int32_ge(const Object& oleft, const Object& oright)
{
    Object oresult;
    oresult.SetType(ObjectType::Bool);

    bool&          result = Object::get<bool>   (oresult);
    const int32_t& left   = Object::get<int32_t>(oleft);
    const int32_t& right  = Object::get<int32_t>(oright);
    result = (left >= right);
    return oresult;
}

operator3 Object::ge_function(ObjectType left, ObjectType right)
{
    if (left == ObjectType::Int32 && right == ObjectType::Int32)
        return Builtin_Int32_Int32_ge;
    return nullptr;
}

