#include "../Object.hpp"

template<>
auto Object::get(const Object& obj) -> const size_t&
{
    if (obj.type == ObjectType::Size)
        return *(size_t*) obj.data;
    else
        throw "incorrect type";
}

template<>
auto Object::get(      Object& obj) -> size_t&
{
    if (obj.type == ObjectType::Size)
        return *(size_t*) obj.data;
    else
        throw "incorrect type";
}

template<>
auto Object::get(const Object& obj) -> const int32_t&
{
    if (obj.type == ObjectType::Int32)
        return *(int32_t*) obj.data;
    else
        throw "incorrect type";
}

template<>
auto Object::get(      Object& obj) -> int32_t&
{
    if (obj.type == ObjectType::Int32)
        return *(int32_t*) obj.data;
    else
        throw "incorrect type";
}

template<>
auto Object::get(const Object& obj) -> const int64_t&
{
    if (obj.type == ObjectType::Int64)
        return *(int64_t*) obj.data;
    else
        throw "incorrect type";
}

template<>
auto Object::get(      Object& obj) -> int64_t&
{
    if (obj.type == ObjectType::Int64)
        return *(int64_t*) obj.data;
    else
        throw "incorrect type";
}

template<>
auto Object::get(const Object& obj) -> const bool&
{
    if (obj.type == ObjectType::Bool)
        return *(bool*) obj.data;
    else
        throw "incorrect type";
}

template<>
auto Object::get(      Object& obj) -> bool&
{
    if (obj.type == ObjectType::Bool)
        return *(bool*) obj.data;
    else
        throw "incorrect type";
}
