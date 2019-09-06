#ifndef _ObjectType_HPP_
#define _ObjectType_HPP_

#include <string>

class ObjectType{
public:
    enum Type{
        None,
        Null,

        Bool,

        Int8,
        Int16,
        Int32,
        Int64,

        UInt8,
        UInt16,
        UInt32,
        UInt64,

        Size,

        String255,
        Exception
    };
public:
    ObjectType()
    {this->type = Type::None;}
    ObjectType(Type type)
    {this->type = type;}
    ObjectType(const ObjectType& other)
    {this->type = other.type;}
public:
    const ObjectType& operator=(Type type)
    {
        this->type = type;
        return *this;
    }
    const ObjectType& operator=(const ObjectType& other)
    {
        if (this == &other) return *this;
        this->type = other.type;
        return *this;
    }
public:
    friend bool operator==(const ObjectType& left, Type right)
    {return left.type == right;}
    friend bool operator!=(const ObjectType& left, Type right)
    {return left.type != right;}
public:
    friend bool operator==(const ObjectType& left, const ObjectType& right)
    {return left.type == right.type;}
    friend bool operator!=(const ObjectType& left, const ObjectType& right)
    {return left.type != right.type;}
public:
    const char* GetName() const
    {return GetTypeName(type);}
    size_t      GetSize() const
    {return GetTypeSize(type);}
public:
    static const char* GetTypeName(Type type);
    static size_t      GetTypeSize(Type type);
public:
    bool isPrimitive()
    {return true;}
public:
    Type type;
};

#endif // _ObjectType_HPP_
