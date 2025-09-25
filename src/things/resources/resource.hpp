#ifndef RESOURCE_H
#define RESOURCE_H

#include "../thing.hpp"
#include "filesystem/file_data.hpp"
#include "theatre/data_t.hpp"

class Resource : public Thing
{
public:
    Resource() = default;
    Resource(const FileData& Data)
    : m_FileData(Data)
    {}

    virtual void SetupVariables(const data_t& data)
    {
        Thing::SetupVariables(data);
        std::string path = "";
        if(data.GetString(path, "File"))
            { m_FileData.LoadFile(path); }
    }

    FileData& Data()
        { return m_FileData; }

protected:
    FileData m_FileData;
};

#endif // RESOURCE_H
