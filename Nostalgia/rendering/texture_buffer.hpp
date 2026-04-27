#ifndef TEXTURE_BUFFER_H
#define TEXTURE_BUFFER_H

#include <Nostalgia/rendering/common.hpp>
#include <Nostalgia/things/resources/image.hpp>

enum TextureType : int
{
    TEXTURE_TYPE_1D,
    TEXTURE_TYPE_2D, TEXTURE_TYPE_CUBE, TEXTURE_TYPE_1D_ARRAY,
    TEXTURE_TYPE_3D, TEXTURE_TYPE_2D_ARRAY
};

enum SamplerFilter : int
{ SAMPLER_FILTER_NEAREST, SAMPLER_FILTER_LINEAR, SAMPLER_FILTER_NONE };

enum SamplerRepeat : int
{
    SAMPLER_REPEAT_MODE_REPEAT,
    SAMPLER_REPEAT_MODE_MIRRORED_REPEAT,
    SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE,
    SAMPLER_REPEAT_MODE_CLAMP_TO_BORDER,
    SAMPLER_REPEAT_MODE_MIRROR_CLAMP_TO_EDGE,
};

struct SamplerState
{
    SamplerFilter min_filter{SAMPLER_FILTER_LINEAR},
        mip_filter_min{SAMPLER_FILTER_NONE},
        mag_filter{SAMPLER_FILTER_LINEAR};
    SamplerRepeat repeat_u{SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE},
        repeat_v{SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE},
        repeat_w{SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE};
    bool use_anisotropy{false};
    float anisotropy_max{1.0f};

    static constinit const SamplerState JuliansPreferredDefaults;

    void SetVariables(Farg<TheatreFile::ThingData> inData);
    void GetVariables(Shared<TheatreFile::ThingData>& ioData) const;
};

inline constinit const SamplerState SamplerState::JuliansPreferredDefaults{
    SAMPLER_FILTER_LINEAR, SAMPLER_FILTER_NONE, SAMPLER_FILTER_NEAREST,
    SAMPLER_REPEAT_MODE_MIRRORED_REPEAT,
    SAMPLER_REPEAT_MODE_MIRRORED_REPEAT,
    SAMPLER_REPEAT_MODE_MIRRORED_REPEAT,
    true, 16.0f
};

struct TextureFormat
{
    constexpr TextureFormat() noexcept = default;
    constexpr TextureFormat(int inWidth, int inHeight, DataFormat inFormat = DATA_FORMAT_SRGB):
        data_format{inFormat}, width{inWidth}, height{inHeight} {}
    constexpr TextureFormat(TextureType inType, int inWidth, int inHeight, DataFormat inFmt = DATA_FORMAT_SRGB):
        type{inType}, data_format{inFmt}, width{inWidth}, height{inHeight} {}

    TextureType type{TEXTURE_TYPE_2D};
    DataFormat data_format{DATA_FORMAT_SRGB};
    int width{1}, height{1}, depth{1}, array_layers{1}, mipmaps{0};

    constexpr bool operator==(Farg<TextureFormat> tf) const
    {
        return type == tf.type and
            data_format == tf.data_format and
            width == tf.width and
            height == tf.height and
            depth == tf.depth and
            array_layers == tf.array_layers and
            mipmaps == tf.mipmaps;
    }
};

struct TextureDataFormat
{
    const uchar* data{nullptr};
    int layer{0}, width{-1}, height{-1}, depth{-1}, xoffset{0}, yoffset{0}, zoffset{0}, lod{0};
};

class TextureBuffer
{
public:
    virtual ~TextureBuffer() = default;

    virtual void GenerateMipmaps() = 0;
    virtual void SetSamplerState(Farg<SamplerState>) = 0;
    virtual void SetData(Farg<TextureDataFormat>) = 0;
    virtual uint GetID() = 0;
    virtual Farg<TextureFormat> GetFormat() = 0;
    virtual Shared<Image> GetImage(int inLayer = 0, int inMipmapLevel = 0) = 0;

    static Shared<TextureBuffer> CreateDummy();
    static Shared<TextureBuffer> Create(Farg<TextureFormat>);
};

class DummyTextureBuffer final : public TextureBuffer
{
public:
    DummyTextureBuffer() = default;
    virtual ~DummyTextureBuffer() = default;

    void GenerateMipmaps() final {}
    void SetSamplerState(Farg<SamplerState>) final {}
    void SetData(Farg<TextureDataFormat>) final {}
    uint GetID() final { return 0; }
    Farg<TextureFormat> GetFormat() final { return _fmt; }
    Shared<Image> GetImage(int = 0, int = 0) final { return Image::CreateEmpty(1, 1); }
    void GetSamplerState(SamplerState&) final {}

private:
    inline static constinit const TextureFormat _fmt{};
};

#endif // TEXTURE_BUFFER_H
