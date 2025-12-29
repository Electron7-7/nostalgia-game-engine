#ifndef RESOURCE_H
#define RESOURCE_H

#include "fwd/filesystem.hpp"
#include "things/thing.hpp"

class Resource : public Thing
{
public:
    Resource();
    Resource(Shared<FileData> inData);

    virtual void SetVariables(Farg<ThingData> inData) override;
    virtual Shared<ThingData> GetVariables() const override;

    Shared<FileData> Data();

protected:
    Shared<FileData> m_pFileData{nullptr};
};

#endif // RESOURCE_H
