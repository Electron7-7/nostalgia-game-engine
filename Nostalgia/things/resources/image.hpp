#ifndef IMAGE_H
#define IMAGE_H

#include <Nostalgia/things/resources/resource.hpp>
#include <Nostalgia/rendering/common.hpp>

class Image : public Resource
{
public:
    static Shared<Image> CreateEmpty(int inWidth, int inHeight, DataFormat inFormat = DATA_FORMAT_SRGB_ALPHA);
    static Shared<Image> CreateFromFile(Farg<FileData> inFile, DataFormat inFormat = DATA_FORMAT_SRGB_ALPHA);
    static Shared<Image> CreateFromData(uchar* inData, int inSize, int inWidth, int inHeight, int inChannels,
        bool inUseMipmaps = true, DataFormat inFormat = DATA_FORMAT_SRGB_ALPHA);
    static void GetInfo(Farg<Shared<FileData>> inFile, int* outWidth, int* outHeight, int* outChannels);

	THING_CLASS(Image, Resource)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE
    SHUTDOWN_OVERRIDE

    virtual ~Image() noexcept;

    Error Import();
    Error LoadFile(Farg<FileData> inFile, DataFormat inFormat = DATA_FORMAT_SRGB_ALPHA);
    void SetData(uchar* inData, int inSize, int inWidth, int inHeight, int inChannels,
        bool inUseMipmaps = true, DataFormat inFormat = DATA_FORMAT_SRGB_ALPHA);
    SafeReturn<std::string> SaveJPG(Sarg inFilePath,
        FileSystem::OverwriteAction inOverwriteAction = FileSystem::RENAME, int inQuality = 100);

    const uchar* data() const;
    uchar* data();
    int size() const;
    Sarg filepath() const;

    bool UseMipmaps() const;
    void SetUseMipmaps(bool inUseMipmaps);
    DataFormat Format() const;
    void SetFormat(DataFormat inFormat);
    int Width() const;
    int Height() const;
    int Channels() const;

protected:
    bool mAllocatedWithSTB{false}, mAllocatedWithNew{false};
    std::string mFilepath{};
    uchar* m_pImage{nullptr};
    int mSize{0};
    DataFormat mFormat{DATA_FORMAT_SRGB_ALPHA};
    bool mUseMipmaps{true};
    int mWidth{1},
        mHeight{1},
        /*
         * Channels is the number of 8-bit components per pixel and is directly related to the format.
         * As an example, if `mChannels` is 4, that would require `mFormat` to be one of the formats where the components are
         * floating-point numbers, which are 32 bits (32 / 4 == 8).
        */
        mChannels{1};

    void free();
};

#endif // IMAGE_H
