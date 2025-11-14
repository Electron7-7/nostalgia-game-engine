#ifndef TEXTURE_H
#define TEXTURE_H

#include "renderer/common.hpp"
#include "resource.hpp"

enum TextureType
{
    TEXTURE_TYPE_1D,
    TEXTURE_TYPE_2D,
    TEXTURE_TYPE_3D,
    TEXTURE_TYPE_CUBE,
    TEXTURE_TYPE_1D_ARRAY,
    TEXTURE_TYPE_2D_ARRAY,
    TEXTURE_TYPE_3D_ARRAY,
    TEXTURE_TYPE_CUBE_ARRAY,
    TEXTURE_TYPE_NONE,
};

enum TextureSamples {
    TEXTURE_SAMPLES_1,
    TEXTURE_SAMPLES_2,
    TEXTURE_SAMPLES_4,
    TEXTURE_SAMPLES_8,
    TEXTURE_SAMPLES_16,
    TEXTURE_SAMPLES_32,
    TEXTURE_SAMPLES_64,
};

struct TextureFormat
{
    RenderingCommons::DataFormat format{RenderingCommons::DATA_FORMAT_NONE};
    uint width{1};
    uint height{1};
    uint depth{1};
    uint array_layers{1};
    uint mipmaps{1};
    TextureType type{TEXTURE_TYPE_2D};
    TextureSamples samples{TEXTURE_SAMPLES_1};
    std::vector<RenderingCommons::DataFormat> shareable_formats{};

    constexpr bool operator==(const TextureFormat& tf) const
    {
        return
            format  == tf.format            &&
            width   == tf.width             &&
            height  == tf.height            &&
            depth   == tf.depth             &&
            mipmaps == tf.mipmaps           &&
            samples == tf.samples           &&
            type    == tf.type              &&
            array_layers == tf.array_layers &&
            shareable_formats == tf.shareable_formats;
    }
};

class Texture : public Resource
{
public:
    using Resource::Resource;

    virtual void SetVariables(const ThingData& data);
    virtual ThingData GetVariables() const;

    TextureFormat format{};
    uint base_mipmap{0};
    uint base_layer{0};
    bool bound_to_framebuffer{false};
};

#endif // TEXTURE_H
