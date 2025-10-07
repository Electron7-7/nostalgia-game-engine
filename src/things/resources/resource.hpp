#ifndef RESOURCE_H
#define RESOURCE_H

#include "../thing.hpp"
#include "filesystem/file_data.hpp"
#include "theatre/thing_data.hpp"

class Resource : public Thing
{
public:
    Resource() = default;
    Resource(const FileData& Data)
    : mFileData(Data)
    {}

    virtual void SetupVariables(const ThingData& data)
    {
        Thing::SetupVariables(data);
        std::string path = "";
        if(data.GetString(path, "File"))
            { mFileData.LoadFile(path); }
    }

    FileData& Data()
        { return mFileData; }

protected:
    FileData mFileData;
};

#endif // RESOURCE_H
