#ifndef RESOURCE_H
#define RESOURCE_H

#include "thing/thing.hpp"
#include "filesystem/file_data.hpp"

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
