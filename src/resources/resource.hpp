#ifndef RESOURCE_H
#define RESOURCE_H

#include "types/typenames.hpp"

#include <string>

typedef unsigned int rid_t;

// TODO: Change/move
enum class ResourceStatus
{
    NOT_PROCESSED,
    FAILED,
    SUCCESSFUL
};

struct Resource
{
public:
    rid_t GetID() const;
    const std::string& GetName() const;
    size_t GetType() const; // FIXME: Probably unnecessary

    static Resource Empty;
    static constexpr rid_t NoRID = 0;

private:
    friend struct ResourceHandler;

    rid_t m_ID = Resource::NoRID;
    std::string m_Name = "Untitled Resource";
    size_t m_Type = Type::Resource; // FIXME: Probably unnecessary
};

#endif // RESOURCE_H
