#ifndef FILE_DATA_H
#define FILE_DATA_H

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
    FileData(const unsigned char* Data, int Size, FileType Type = FileType::Unknown);
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
    std::vector<uchar> mData{};
    std::string mPath{};
    FileType mType{FileType::Unknown};
    Error mStatus{ERR_EMPTY};
};

#endif // FILE_DATA_H
