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
    virtual ~Resource() = default;

    rid_t GetID() const;
    size_t GetType() const;
    const std::string& GetName() const;

    static Resource Empty;
    static constexpr rid_t NoRID = 0;

private:
    friend class TheatreManager;

    rid_t m_ID = Resource::NoRID;
    size_t m_Type = Type::Resource;
    std::string m_Name = "Untitled Resource";
};

template<typename T>
concept IsResource = requires
{ std::derived_from<T, Resource> == true; };

#endif // RESOURCE_H
