#include "../Object.hpp"

static void Object_Builtin_Int32_Int32_add_assignment(Object& oleft, const Object& oright)
{
          int32_t& left  = Object::get<int32_t>(oleft);
    const int32_t& right = Object::get<int32_t>(oright);
    left += right;
}

static void Object_Builtin_Int64_Int64_add_assignment(Object& oleft, const Object& oright)
{
          int64_t& left  = Object::get<int64_t>(oleft);
    const int64_t& right = Object::get<int64_t>(oright);
    left += right;
}

static void Object_Builtin_Size_Size_add_assignment(Object& oleft, const Object& oright)
{
          size_t& left  = Object::get<size_t>(oleft);
    const size_t& right = Object::get<size_t>(oright);
    left += right;
}

operator2 Object::add2_function(ObjectType left, ObjectType right)
{
    if ((left == ObjectType::Int32) && (right == ObjectType::Int32))
        return Object_Builtin_Int32_Int32_add_assignment;
    if ((left == ObjectType::Int64) && (right == ObjectType::Int64))
        return Object_Builtin_Int64_Int64_add_assignment;
    if ((left == ObjectType::Size)  && (right == ObjectType::Size))
        return Object_Builtin_Size_Size_add_assignment;
    return nullptr;
}

