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

    constexpr FileData() = default;
    constexpr FileData(FileType type, auto& data)
    : m_Data(data), m_Size(sizeof data), m_Type(type), m_Status(DataStatus::SUCCESSFUL), m_ReleaseData(false)
    {}

    FileData(const std::string& path, FileType type = FileType::Unknown)
    : m_Path(path), m_Data(nullptr), m_Size(0), m_Type(type), m_Status(DataStatus::UNLOADED), m_ReleaseData(false)
    { LoadFile(path, type); }
    FileData(const FileData& copy)
    : m_Path(copy.m_Path), m_Data(copy.m_Data), m_Type(copy.m_Type), m_Status(copy.m_Status), m_ReleaseData(copy.m_ReleaseData)
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
    std::string String() const;

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
