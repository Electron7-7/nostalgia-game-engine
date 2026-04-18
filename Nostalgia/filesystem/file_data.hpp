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
    using Data_t = std::vector<uchar>;

    static FileType sDetectFileType(Sarg inPath);

    FileData();
    FileData(const uchar* inData, size_t inSize, FileType inType = FileType::Unknown, Sarg inOverridePath = "");
    FileData(Sarg inStrData, FileType inType, Sarg inOverridePath = "");
    FileData(Sarg inPath);

    void  SetData(const uchar* inData, size_t inSize, FileType inType = FileType::Unknown);
    Error ReadFile(Sarg inPath);
    SafeReturn<std::string> WriteFile(Sarg inPath, FileSystem::OverwriteAction = FileSystem::RENAME);

    void                clear();
    Data_t&              data();
    Farg<Data_t>         data() const;
    uchar*           raw_data();
    const uchar*     raw_data() const;
    std::string  raw_data_str() const;
    size_t               size() const;
    Error              status() const;
    FileType        file_type() const;
    Sarg             filepath() const;
    bool         has_filepath() const;
    bool                empty() const;

private:
    Data_t mData{};
    std::string mPath{};
    FileType mType{FileType::Unknown};
    Error mStatus{ERR_EMPTY};
};

#endif // FILE_DATA_H
