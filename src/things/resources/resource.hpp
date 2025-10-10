#ifndef RESOURCE_H
#define RESOURCE_H

#include "../thing.hpp"
#include "filesystem/file_data.hpp"
#include "theatre_parser/thing_data.hpp"

class Resource : public Thing
{
public:
    Resource() = default;
    Resource(const FileData& Data)
        : mFileData(Data) {}

    virtual void SetVariables(const ThingData& data)
    {
        Thing::SetVariables(data);
        std::string path = "";
        if(data.GetVariable(path, "File"))
            { mFileData.LoadFile(path); }
    }

    virtual ThingData GetVariables() const
    {
        ThingData thing_data{Thing::GetVariables()};
        thing_data.AddVariable(mFileData.Path(), "File");
        return thing_data;
    }

    FileData& Data()
        { return mFileData; }

protected:
    FileData mFileData;
};

#endif // RESOURCE_H
