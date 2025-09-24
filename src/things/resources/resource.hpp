#ifndef RESOURCE_H
#define RESOURCE_H

#include "../thing.hpp"
#include "safe_return.hpp"
#include "filesystem/file_data.hpp"

enum class ResourceStatus
{
    NOT_PROCESSED,
    FAILED,
    SUCCESSFUL
};

class Resource : public Thing
{
public:
    Resource();
    Resource(const FileData& Data);

    virtual SafeStatus CreateResource() { return Status::NO_ERR; };
    virtual void SetupVariables(const data_t&);
    virtual bool Initialize() { return true; }
    virtual void Destroy() {};
    virtual FileData& Data() { return m_FileData; }
    virtual ResourceStatus Status() { return m_Status; }

protected:
    FileData m_FileData;
    ResourceStatus m_Status = ResourceStatus::NOT_PROCESSED;
};

#endif // RESOURCE_H
