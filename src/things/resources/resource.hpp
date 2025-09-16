#ifndef RESOURCE_H
#define RESOURCE_H

#include "../thing.hpp"
#include "filesystem/file_data.hpp"

#include <memory>

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
    Resource();
    Resource(const FileData* Data);

    virtual SafeStatus CreateResource() { return Status::NO_ERR; };
    virtual void SetupVariables(const data_t&);
    virtual bool Initialize() { return true; }
    virtual void Destroy() {}
    virtual const std::shared_ptr<const FileData> Data() const { return m_FileData; }

private:
    std::shared_ptr<const FileData> m_FileData = std::make_shared<FileData>();
};

#endif // RESOURCE_H
