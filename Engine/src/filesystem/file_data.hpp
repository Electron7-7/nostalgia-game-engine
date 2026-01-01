#ifndef FILE_DATA_H
#define FILE_DATA_H

#include "core/smart_pointers.hpp"
#include "fwd/core.hpp"
#include "core/farg.hpp"
#include <string>

enum class FileType : ushort
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

#pragma message("TODO: change this to use `Error`")
enum class DataStatus : ushort
{
    UNLOADED,
    FAILED,
    EMPTY,
    SUCCESSFUL
};

struct FileData
{
public:
    static FileType s_DetectFileType(Farg<std::string> FilePath);
    static Shared<FileData> s_GetReservedFileData(uint inReservedUID);

    FileData();
    FileData(const unsigned char* Data, int Size, FileType Type);
    FileData(Farg<std::string> Path, FileType Type = FileType::Unknown);
    ~FileData();

    const unsigned char* Data() const;
    int Size() const;
    bool Empty() const;
    DataStatus Status() const;
    FileType Type() const;
    Farg<std::string> Path() const;
    bool HasPath() const;

    void Clear();

    Error LoadFile(Farg<std::string> Path, FileType Type = FileType::Unknown);
    void LoadData(const unsigned char* Data, int Size, FileType Type);

    std::string String() const;

private:
    std::string m_Path{};
    const unsigned char* m_Data{nullptr};
    int m_Size{0};
    FileType m_Type{FileType::Unknown};
    DataStatus m_Status{DataStatus::EMPTY};
    bool m_ReleaseData{false};
};

#endif // FILE_DATA_H
