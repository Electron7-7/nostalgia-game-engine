#ifndef RESOURCE_DATA_H
#define RESOURCE_DATA_H

#include "filesystem/fwd.hpp"

#include <string>

struct ResourceData
{
public:
    static bool Exists(const std::string& Name);
    static void AddData(const std::string& Name, const FileData& Data);
    static bool GetData(FileData& Output, const std::string& Name);
};

#endif // RESOURCE_DATA_H
