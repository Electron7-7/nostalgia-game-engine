#include "gl_texture_buffer.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb_image/stb_image.h"
#include <glad/glad.h>

static GLenum s_GLType(TextureType inType)
{
    switch(inType)
    {
    case TEXTURE_TYPE_1D:
        return GL_TEXTURE_1D;
    case TEXTURE_TYPE_2D:
        return GL_TEXTURE_2D;
    case TEXTURE_TYPE_3D:
        return GL_TEXTURE_3D;
    case TEXTURE_TYPE_1D_ARRAY:
        return GL_TEXTURE_1D_ARRAY;
    case TEXTURE_TYPE_2D_ARRAY:
        return GL_TEXTURE_2D_ARRAY;
    case TEXTURE_TYPE_CUBE_ARRAY:
        return GL_TEXTURE_CUBE_MAP_ARRAY;
    case TEXTURE_TYPE_CUBE:
        return GL_TEXTURE_CUBE_MAP;
    case TEXTURE_TYPE_NONE:
    default:
        return GL_TEXTURE_2D;
    }
}

static GLint s_GLInternalFormat(DataFormat inFormat)
{
    switch(inFormat)
    {
    case DATA_FORMAT_RGB:
        return GL_RGB8;
    case DATA_FORMAT_RGBA:
    case DATA_FORMAT_RGBA8:
        return GL_RGBA8;
    case DATA_FORMAT_SRGB_ALPHA:
        return GL_SRGB_ALPHA;
    case DATA_FORMAT_NONE:
    default:
        print_warning("Invalid data format! Returning `GL_NONE` (this will probably cause a crash!)");
        return GL_NONE;
    }
}

static GLenum s_GLFormat(DataFormat inFormat)
{
    switch(inFormat)
    {
    case DATA_FORMAT_RGB:
        return GL_RGB;
    case DATA_FORMAT_RGBA:
    case DATA_FORMAT_RGBA8:
        return GL_RGBA;
    case DATA_FORMAT_SRGB_ALPHA:
        return GL_RGB;
    case DATA_FORMAT_NONE:
    default:
        print_warning("Invalid data format! Returning `GL_NONE` (this will probably cause a crash!)");
        return GL_NONE;
    }
}

static GLenum s_GLFilter(SamplerFilter inFilter, SamplerFilter inMipFilter)
{
    if(inFilter == SAMPLER_FILTER_NONE)
        { return GL_LINEAR; } // idk if `GL_NONE` is valid so just return `GL_LINEAR`
    uint has_mip = (inMipFilter != SAMPLER_FILTER_NONE)
        ? 0b100000000
        : 0b0;
    uint out{GL_NEAREST | has_mip};
    if(inMipFilter == SAMPLER_FILTER_LINEAR)
        { out += 2; }
    if(inFilter == SAMPLER_FILTER_LINEAR)
        { ++out; }
    return out;
}

static GLenum s_GLWrap(SamplerRepeat inRepeat)
{
    switch(inRepeat)
    {
        case SAMPLER_REPEAT_MODE_REPEAT:
            return GL_REPEAT;
        case SAMPLER_REPEAT_MODE_MIRRORED_REPEAT:
            return GL_MIRRORED_REPEAT;
        case SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE:
            return GL_CLAMP_TO_EDGE;
        case SAMPLER_REPEAT_MODE_CLAMP_TO_BORDER:
            return GL_CLAMP_TO_BORDER;
        case SAMPLER_REPEAT_MODE_MIRROR_CLAMP_TO_EDGE:
            return GL_MIRROR_CLAMP_TO_EDGE;
    }
}

static Error s_GenerateTexture(uint& ioID,
    TextureFormat& ioFormat,
    SamplerState& ioSampler,
    Farg<cubemap_images_t> inData)
{
    GLenum gl_type{s_GLType(ioFormat.type)};

    glGenTextures(1, &ioID);
    glBindTexture(gl_type, ioID);

    uint i{0};
    for(Farg<Shared<FileData>> data : inData)
    {
        unsigned char* image_data{nullptr};
        if(data)
        {
            stbi_set_flip_vertically_on_load(true);

            image_data = stbi_load_from_memory(data->Data(),
                data->Size(),
                &ioFormat.width,
                &ioFormat.height,
                &ioFormat.channels,
                STBI_rgb);
            if(!image_data)
            {
                print_warning("STBI failed to load image data for TextureBuffer#{}", ioID);
                stbi_image_free(image_data);
                return ERR_DATA_LOAD;
            }
        }
        switch(gl_type)
        {
        case GL_TEXTURE_2D:
            glTexImage2D(GL_TEXTURE_2D,
                0,
                s_GLInternalFormat(ioFormat.data_format),
                ioFormat.width,
                ioFormat.height,
                0,
                s_GLFormat(ioFormat.data_format),
                GL_UNSIGNED_BYTE,
                image_data);
            break;
        case GL_TEXTURE_CUBE_MAP:
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                s_GLInternalFormat(ioFormat.data_format),
                ioFormat.width,
                ioFormat.height,
                0,
                s_GLFormat(ioFormat.data_format),
                GL_UNSIGNED_BYTE,
                image_data);
            break;
        default:
            print_warning("The requested OpenGL texture format is unimplemented, sorry");
            stbi_image_free(image_data);
            return UNIMPLEMENTED;
        }
        stbi_image_free(image_data);
        ++i;
    }
    if(ioSampler.mip_filter_min != SAMPLER_FILTER_NONE)
        { glGenerateTextureMipmap(ioID); }
    glTextureParameteri(ioID, GL_TEXTURE_WRAP_S, s_GLWrap(ioSampler.repeat_u));
    glTextureParameteri(ioID, GL_TEXTURE_WRAP_T, s_GLWrap(ioSampler.repeat_v));
    glTextureParameteri(ioID, GL_TEXTURE_WRAP_R, s_GLWrap(ioSampler.repeat_w));
    glTextureParameterf(ioID, GL_TEXTURE_MAX_ANISOTROPY, ioSampler.use_anisotropy * ioSampler.anisotropy_max);
    glTextureParameteri(ioID, GL_TEXTURE_MIN_FILTER, s_GLFilter(ioSampler.min_filter, ioSampler.mip_filter_min));
    glTextureParameteri(ioID, GL_TEXTURE_MAG_FILTER, s_GLFilter(ioSampler.mag_filter, SAMPLER_FILTER_NONE));
    glBindTexture(gl_type, 0);
    return OK;
}

OpenGLTextureBuffer::OpenGLTextureBuffer(Farg<TextureFormat> inFormat,
    Farg<SamplerState> inSampler,
    Farg<cubemap_images_t> inData):
        mFormat{inFormat},
        mSampler{inSampler},
        mStatus{s_GenerateTexture(mBufferID, mFormat, mSampler, inData)} {}

OpenGLTextureBuffer::~OpenGLTextureBuffer()
{ glDeleteTextures(1, &mBufferID); }

Error OpenGLTextureBuffer::Status() const
{ return mStatus; }

uint OpenGLTextureBuffer::ID() const
{ return mBufferID; }

Farg<TextureFormat> OpenGLTextureBuffer::Format() const
{ return mFormat; }

Farg<SamplerState> OpenGLTextureBuffer::Sampler() const
{ return mSampler; }
