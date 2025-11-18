#ifndef FILE_DATA_H
#define FILE_DATA_H

#include "core/error.hpp"

#include <string>

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
    FileData(const unsigned char* Data, int Size, FileType Type);
    FileData(const std::string& Path, FileType Type = FileType::Unknown);
    ~FileData();

    const unsigned char* Data() const;
    int Size() const;
    bool Empty() const;
    DataStatus Status() const;
    FileType Type() const;
    const std::string& Path() const;
    bool HasPath() const;

    void Clear();

    Error LoadFile(const std::string& Path, FileType Type = FileType::Unknown);
    void LoadData(const unsigned char* Data, int Size, FileType Type);

    std::string String() const;

private:
    std::string m_Path = "";
    const unsigned char* m_Data = nullptr;
    int m_Size = 0;
    FileType m_Type = FileType::Unknown;
    DataStatus m_Status = DataStatus::EMPTY;
    bool m_ReleaseData = false;
};

#endif // FILE_DATA_H
