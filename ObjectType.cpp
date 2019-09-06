#include "ObjectType.hpp"

const char*
ObjectType::GetTypeName(Type type)
{
    switch(type)
    {
        case ObjectType::None:
            return "ObjectType::None";
        case ObjectType::Null:
            return "ObjectType::Null";
        case ObjectType::Bool:
            return "ObjectType::Bool";
        case ObjectType::Int8:
            return "ObjectType::Int8";
        case ObjectType::Int16:
            return "ObjectType::Int16";
        case ObjectType::Int32:
            return "ObjectType::Int32";
        case ObjectType::Int64:
            return "ObjectType::Int64";
        case ObjectType::UInt8:
            return "ObjectType::UInt8";
        case ObjectType::UInt16:
            return "ObjectType::UInt16";
        case ObjectType::UInt32:
            return "ObjectType::UInt32";
        case ObjectType::UInt64:
            return "ObjectType::UInt64";
        case ObjectType::Size:
            return "ObjectType::Size";
        default:
            return "Unknown";
    }
}

size_t ObjectType::GetTypeSize(Type type)
{
    switch(type)
    {
        case ObjectType::None:
            return 0;
        case ObjectType::Null:
            return 0;
        case ObjectType::Bool:
            return sizeof(bool);
        case ObjectType::Int8:
            return sizeof(int8_t);
        case ObjectType::Int16:
            return sizeof(int16_t);
        case ObjectType::Int32:
            return sizeof(int32_t);
        case ObjectType::Int64:
            return sizeof(int64_t);
        case ObjectType::UInt8:
            return sizeof(uint8_t);
        case ObjectType::UInt16:
            return sizeof(uint16_t);
        case ObjectType::UInt32:
            return sizeof(uint32_t);
        case ObjectType::UInt64:
            return sizeof(uint64_t);
        case ObjectType::Size:
            return sizeof(size_t);
        case ObjectType::String255:
            return 256;
        default:
            return 0;
    }
}

