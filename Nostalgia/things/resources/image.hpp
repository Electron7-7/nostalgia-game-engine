#ifndef IMAGE_H
#define IMAGE_H

#include <Nostalgia/rendering/common.hpp>

class Image : public Resource
{
public:
    static Shared<Image> CreateEmpty(int inWidth, int inHeight, bool inUseMipmaps, DataFormat inFormat);
    static Shared<Image> CreateFromData(int inWidth, int inHeight, bool inUseMipmaps, DataFormat inFormat,
        uchar* inData, int inSize);

    Farg<Shared<FileData>> Data() const;
    Shared<FileData> Data();
    bool UseMipmaps() const;
    int Width() const;
    int Height() const;
    DataFormat Format() const;
    int STBI_NumberOf8BitComponents() const;

    SUPER(Resource);
    SET_VARIABLES_OVERRIDE;
    GET_VARIABLES_OVERRIDE;
    virtual void Shutdown() override;

    virtual Error Load(Sarg inFilePath);
    virtual void SetData(int inWidth, int inHeight, bool inUseMipmaps, DataFormat inFormat,
        uchar* inData, int inSize);

protected:
    // FileData size post-stbi is calculated by mWidth * mHeight * mChannels
    Shared<FileData> mData{MakeShared<FileData>()};
    DataFormat mFormat{DATA_FORMAT_RED};
    bool mUseMipmaps{false};
    int mWidth{1},
        mHeight{1},
        /*
         * Channels is the number of 8-bit components per pixel and is directly related to the format.
         * As an example, if `mChannels` is 4, that would require `mFormat` to be one of the formats where the components are
         * floating-point numbers, which are 32 bits (32 / 4 == 8).
        */
        mChannels{1};
};

#endif // IMAGE_H
