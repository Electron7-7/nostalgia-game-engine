#ifndef FILE_DATA_H
#define FILE_DATA_H

#ifdef NOSTALGIA_GCC_PCH
#   include <Nostalgia/Nostalgia.hpp>
#endif

enum class FileType : ushort
{
    Unknown = 0,
    theatre_NT,
    model_OBJ,
    image_PNG,
    image_JPG,
    font_TTF,
    font_OTF,
    glsl_FRAG,
    glsl_VERT,
};

struct FileData
{
public:
    static FileType s_DetectFileType(Farg<std::string> FilePath);

    FileData();
    FileData(const unsigned char* Data, int Size, FileType Type);
    FileData(Farg<std::string> Path, FileType Type = FileType::Unknown);
    ~FileData();

    const unsigned char* Data() const;
    std::string DataString() const;

    int Size() const;
    Error Status() const;
    FileType Type() const;
    Farg<std::string> Path() const;
    bool HasPath() const;

    Error LoadFile(Farg<std::string> Path, FileType Type = FileType::Unknown);
    void LoadData(const unsigned char* Data, int Size, FileType Type);

    bool empty() const;
    void clear();

private:
    const unsigned char* mData{nullptr};
    int mSize{0};
    std::string mPath{};
    FileType mType{FileType::Unknown};
    Error mStatus{ERR_EMPTY};
    bool mReleaseData{false};
};

#endif // FILE_DATA_H
