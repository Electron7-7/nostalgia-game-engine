#ifndef RESOURCE_H
#define RESOURCE_H

#include "../thing.hpp"
#include "theatre/theatre_data.hpp"

// TODO: Change/move
enum class ResourceStatus
{
    NOT_PROCESSED,
    FAILED,
    SUCCESSFUL
};

class Resource : public Thing
{
public:
    virtual void SetupVariables(const data_t&) {};
    virtual bool Initialize() { return true; }
    virtual void Destroy() {}
};

#endif // RESOURCE_H
