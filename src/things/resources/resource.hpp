#ifndef RESOURCE_H
#define RESOURCE_H

#include "../thing.hpp"
#include "safe_return.hpp"

struct FileData; // Forward Declaration

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

    const std::shared_ptr<const FileData> Data() const { return m_FileData; }
    void SetData(std::shared_ptr<const FileData> NewData) { m_FileData = NewData; }

private:
    std::shared_ptr<const FileData> m_FileData = nullptr;
};

#endif // RESOURCE_H
