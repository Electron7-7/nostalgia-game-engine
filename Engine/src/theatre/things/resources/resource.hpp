#ifndef RESOURCE_H
#define RESOURCE_H

#include "theatre/things/thing.hpp"
#include "filesystem/file_data.hpp"

// Similar to either Godot's `Resource` or `RefCounted` (I haven't decided yet)
class Resource : public Thing
{
public:
    virtual void SetVariables(Farg<ThingData> inData) override;
    virtual Shared<ThingData> GetVariables() const override;

    Error Status() const;

    Shared<FileData> Data();

protected:
    Shared<FileData> m_pFileData{MakeShared<FileData>()};
    Error mStatus;
};

#endif // RESOURCE_H
