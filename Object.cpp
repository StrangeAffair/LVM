#include "Object.hpp"

void Object::move(Object& dest, Object& src)
{
    dest.type = src.type;
    dest.data = src.data;

    src.type = ObjectType::None;
    src.data = nullptr;
}

void Object::copy(Object& dest, const Object& src)
{
    ObjectType type = src.type;
    if (type.isPrimitive())
    {
        dest.SetType(type);
        memcpy(dest.data, src.data, type.GetSize());
    }
}

void Object::clear(Object& dest)
{
    if (dest.data != nullptr)
        free(dest.data);
    dest.type = ObjectType::None;
    dest.data = nullptr;
}

