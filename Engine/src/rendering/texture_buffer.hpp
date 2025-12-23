#ifdef FWD_DCL
    enum TextureType : unsigned short;
    class TextureBuffer;
#elif !defined TEXTURE_BUFFER_H
#define TEXTURE_BUFFER_H

#define FWD_DCL
#   include "filesystem/file_data.hpp"
#undef  FWD_DCL

#include "core/farg.hpp"
#include "core/error.hpp"
#include "core/smart_pointers.hpp"

enum TextureType : ushort
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

struct TextureFormat
{
    TextureFormat() noexcept = default;
    TextureFormat(TextureType inType,
        int inWidth,
        int inHeight,
        int inChannels = 1,
        uint inDepth = 1,
        uint inLayers = 1,
        uint inMipmaps = 1):
        type{inType},
        width{inWidth},
        height{inHeight},
        channels{inChannels},
        depth{inDepth},
        array_layers{inLayers},
        mipmaps{inMipmaps} {}

    TextureType type{TEXTURE_TYPE_2D};
    int  width{1};
    int  height{1};
    int  channels{1};
    uint depth{1};
    uint array_layers{1};
    uint mipmaps{1};

    constexpr bool operator==(const TextureFormat& tf) const
    {
        return
            width   == tf.width             &&
            height  == tf.height            &&
            depth   == tf.depth             &&
            mipmaps == tf.mipmaps           &&
            type    == tf.type              &&
            array_layers == tf.array_layers;
    }
};

class TextureBuffer
{
public:
    virtual ~TextureBuffer() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void BindUnit(uint inTextureUnit) = 0;
    virtual void UnbindUnit() const = 0;

    virtual uint GetID() const = 0;
    virtual Farg<TextureFormat> GetFormat() const = 0;

    virtual Error GenerateTexture(Farg<TextureFormat> = {}) = 0;
    virtual Error GenerateTexture(Farg<FileData>, Farg<TextureFormat> = {}) = 0;

    static Shared<TextureBuffer> Create();
};

#endif // TEXTURE_BUFFER_H
