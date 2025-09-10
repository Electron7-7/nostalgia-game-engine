#ifndef RESOURCE_H
#define RESOURCE_H

#include "things/id.hpp"
#include "theatre/theatre_data.hpp"

// TODO: Change/move
enum class ResourceStatus
{
    NOT_PROCESSED,
    FAILED,
    SUCCESSFUL
};

struct _resource
{
public:
    virtual ~_resource() = default;

    virtual void SetupVariables(const data_t&) = 0;
    virtual bool Initialize() = 0;
    virtual void Destroy() = 0;

    id_t GetID() const;
    const std::string& GetName() const;
    size_t GetType() const;

private:
    friend class TheatreManager;
    id_t m_ID = NoID;
    std::string m_Name = "Untitled Resource";
    size_t m_Type = Type::Resource;
};

struct Resource : public _resource
{
public:
    virtual void SetupVariables(const data_t&) {};
    virtual bool Initialize() { return true; }
    virtual void Destroy() {}
};

template<typename T>
concept IsResource = requires
{ std::derived_from<T, Resource> == true; };

#endif // RESOURCE_H
