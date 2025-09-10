#ifndef RESOURCE_DATA_H
#define RESOURCE_DATA_H

#include "safe_return.hpp"
#include "filesystem/binary_file_data.hpp"

struct ResourceData
{
public:
    static bool Exists(const std::string& Name);
    static bool try_Add(const std::string& Name, const BinaryFileData& BinaryData);
    static bool try_Add(const std::string& Name, const StringFileData& StringData);
    static SafeReturn<const BinaryFileData&> try_GetBinaryData(const std::string& Name);
    static SafeReturn<const StringFileData&> try_GetStringData(const std::string& Name);
};
#endif // RESOURCE_DATA_H
