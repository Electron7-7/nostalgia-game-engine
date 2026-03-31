#ifndef IMAGE_H
#define IMAGE_H

#include <Nostalgia/things/resources/resource.hpp>
#include <Nostalgia/rendering/common.hpp>

class Image : public Resource
{
public:
    static Shared<Image> CreateEmpty(int inWidth, int inHeight, bool inUseMipmaps, DataFormat inFormat);
    static Shared<Image> CreateFromData(bool inUseMipmaps, DataFormat inFormat,
        const uchar* inImageData, int inImageDataSize);
    static void GetInfo(Farg<Shared<FileData>> inFile, int* outWidth, int* outHeight, int* outChannels);

	SET_SUPER(Resource)
	SET_TYPEID(ThingType::Image)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE
    SHUTDOWN_OVERRIDE

    virtual ~Image() noexcept;

    Error Import();
    Error LoadFile(Sarg inFilePath);
    void SetData(bool inUseMipmaps, DataFormat inFormat, const uchar* inData, int inSize);

    const uchar* raw_data() const;
    uchar* raw_data();
    int size() const;

    bool UseMipmaps() const;
    void SetUseMipmaps(bool inUseMipmaps);
    DataFormat Format() const;
    void SetFormat(DataFormat inFormat);
    int Width() const;
    int Height() const;
    int Channels() const;

protected:
    Shared<FileData> m_pFileData{MakeShared<FileData>()};
    uchar* m_pImage{nullptr};
    int mSize{0};
    DataFormat mFormat{DATA_FORMAT_SRGB_ALPHA};
    bool mUseMipmaps{false};
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
