#ifdef FWD_DCL
    class Resource;
#elif !defined RESOURCE_H
#define RESOURCE_H

#define FWD_DCL
#   include "theatre/parser/thing_data.hpp"
#   include "filesystem/file_data.hpp"
#undef  FWD_DCL

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
