#ifndef BINARY_FILE_DATA_H
#define BINARY_FILE_DATA_H

#include <string>
#include <vector>

typedef std::string StringFileData;

struct BinaryFileData
{
public:
    BinaryFileData(const std::vector<unsigned char>& Data);
    BinaryFileData(unsigned char* Data, unsigned int Size);
    BinaryFileData();

    const std::vector<unsigned char>& GetData() const;

private:
    std::vector<unsigned char> m_Data = {};
};

#endif // BINARY_FILE_DATA_H
