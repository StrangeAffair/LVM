#include "../Object.hpp"

template<>
void Object::set(Object& obj, const int32_t& value)
{
    obj.SetType(ObjectType::Int32);
    Object::get<int32_t>(obj) = value;
}

template<>
void Object::set(Object& obj, const int64_t& value)
{
    obj.SetType(ObjectType::Int64);
    Object::get<int64_t>(obj) = value;
}

template<>
void Object::set(Object& obj, const size_t&  value)
{
    obj.SetType(ObjectType::Size);
    Object::get<size_t> (obj) = value;
}
