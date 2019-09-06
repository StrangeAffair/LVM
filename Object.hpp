#ifndef _Object_HPP_
#define _Object_HPP_ 1

#include "ObjectType.hpp"
#include <cstdint>
#include <cstring>

/*
class Object;

typedef Object (*operator3)(const Object&, const Object&);
typedef void   (*operator2)(      Object&, const Object&);
*/

///implemets COW(Copy on Write) Objects
class Object
{
private:
    const Object& operator=(const Object&);
//private:
public:
    ObjectType type;
    void*      data;
public:
    void SetType(ObjectType type)
    {
        if (this->type != ObjectType::None)
            free(this->data);
        this->type = type;
        this->data = malloc(type.GetSize());
    }
    ObjectType GetType() const
    {
        return type;
    }
public:
    void set();
public:
    template<typename T>
    static void set(Object& obj, const T& value);
public:
    template<typename T>
    static auto get(const Object& obj) -> const T&;
    template<typename T>
    static auto get(      Object& obj) -> T&;
public:
    static void move (Object& dest,       Object& src);
    static void copy (Object& dest, const Object& src);
    static void clear(Object& dest);
/*
public:
    static operator2 add2_function(ObjectType left, ObjectType right);
    static operator2 mul2_function(ObjectType left, ObjectType right);
    static operator3   eq_function(ObjectType left, ObjectType right);
    static operator3   ne_function(ObjectType left, ObjectType right);
    static operator3   lt_function(ObjectType left, ObjectType right);
    static operator3   le_function(ObjectType left, ObjectType right);
    static operator3   gt_function(ObjectType left, ObjectType right);
    static operator3   ge_function(ObjectType left, ObjectType right);
*/
};

#endif // _Object_HPP_

