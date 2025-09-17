#ifndef FILE_DATA_H
#define FILE_DATA_H

#include "safe_return.hpp"

#include <string>
#include <vector>

enum class FileType
{
    Unknown = 0,
    model_OBJ,
    image_PNG,
    image_JPG,
    font_TTF,
    font_OTF,
    glsl_FRAG,
    glsl_VERT,
};

enum class DataStatus
{
    UNLOADED,
    FAILED,
    EMPTY,
    SUCCESSFUL
};

struct FileData
{
public:
    static FileType s_DetectFileType(const std::string& FilePath);

    FileData();
    FileData(const std::string& Path, FileType Type = FileType::Unknown);

    constexpr FileData(FileType type, const unsigned char (&data)[], long size)
    : m_Data(data), m_Size(size), m_Type(type), m_Status(DataStatus::SUCCESSFUL), m_ReleaseData(false)
    {}
    constexpr ~FileData()
    {
        if(m_ReleaseData)
            { delete [] m_Data; }
    }

    DataStatus Status() const;

    FileType Type() const;

    bool HasPath() const;
    const std::string& Path() const;

    std::vector<unsigned char> Vector() const; // Slow
    std::string String() const; // Slow?

    const unsigned char* data() const;
    long size() const;
    bool empty() const;

    std::string m_Path = "";
private:
    SafeStatus LoadFile(const std::string& Path, FileType Type = FileType::Unknown);

    const unsigned char* m_Data = nullptr;
    long m_Size = 0;
    FileType m_Type = FileType::Unknown;
    DataStatus m_Status = DataStatus::EMPTY;
    bool m_ReleaseData = false;
};

#endif // FILE_DATA_H
