#ifndef TEXTURE_BUFFER_H
#define TEXTURE_BUFFER_H

#include "rendering/common.hpp"

enum TextureType : int
{
    TEXTURE_TYPE_1D,       TEXTURE_TYPE_2D,         TEXTURE_TYPE_3D,
    TEXTURE_TYPE_1D_ARRAY, TEXTURE_TYPE_2D_ARRAY,   TEXTURE_TYPE_3D_ARRAY,
    TEXTURE_TYPE_CUBE,     TEXTURE_TYPE_CUBE_ARRAY,
    TEXTURE_TYPE_NONE
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
        mip_filter_min{SAMPLER_FILTER_LINEAR},
        mag_filter{SAMPLER_FILTER_NEAREST};
    SamplerRepeat repeat_u{SAMPLER_REPEAT_MODE_REPEAT},
        repeat_v{SAMPLER_REPEAT_MODE_REPEAT},
        repeat_w{SAMPLER_REPEAT_MODE_REPEAT};
    bool use_anisotropy{false};
    float anisotropy_max{1.0f};

    static constinit const SamplerState JuliansPreferredDefaults;
};

inline constinit const SamplerState SamplerState::JuliansPreferredDefaults{
    SAMPLER_FILTER_LINEAR, SAMPLER_FILTER_LINEAR, SAMPLER_FILTER_NEAREST,
    SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE,
    SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE,
    SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE,
    true, 16.0f
};

struct TextureFormat
{
    constexpr TextureFormat() noexcept = default;
    constexpr TextureFormat(int inWidth, int inHeight, DataFormat inDataFormat = DATA_FORMAT_SRGB_ALPHA):
        data_format{inDataFormat},
        width{inWidth},
        height{inHeight} {}

    TextureType type{TEXTURE_TYPE_2D};
    DataFormat data_format{DATA_FORMAT_SRGB_ALPHA};
    int  width{1}, height{1}, channels{0};
    uint depth{1}, array_layers{1}, mipmaps{1};

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

using cubemap_images_t = std::array<Shared<FileData>, 6>;

class TextureBuffer
{
public:
    virtual ~TextureBuffer() = default;

    virtual Error Status() const = 0;
    virtual uint ID() const = 0;
    virtual Farg<TextureFormat> Format() const = 0;
    virtual Farg<SamplerState> Sampler() const = 0;

    static Shared<TextureBuffer> Create(Farg<TextureFormat>, Farg<SamplerState> = {});
    static Shared<TextureBuffer> Create(Farg<TextureFormat>, Farg<SamplerState>, Shared<FileData>);
    static Shared<TextureBuffer> Create(Farg<TextureFormat>, Farg<SamplerState>, Farg<cubemap_images_t>);
};

#endif // TEXTURE_BUFFER_H
