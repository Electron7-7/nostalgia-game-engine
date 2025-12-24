#include "gl_texture_buffer.hpp"
#include "core/printing.hpp"
#include "filesystem/file_data.hpp"
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

/*
Error OpenGLTextureBuffer::GenerateTexture(Farg<TextureFormat> inTextureFormat)
{
    GLenum gl_type{s_GLType(mTextureFormat.type)};
    glCreateTextures(gl_type, 1, &mBufferID);
    glBindTexture(gl_type, mBufferID);
    switch(gl_type)
    {
    case GL_TEXTURE_2D:
        glTexImage2D(gl_type,
            0,
            GL_RGBA8,
            mTextureFormat.width,
            mTextureFormat.height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            nullptr);
        break;
    default:
        glDeleteTextures(1, &mBufferID);
        print_error("Unimplemented OpenGL texture format");
        return UNIMPLEMENTED;
    }
    glTexParameteri(gl_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(gl_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(gl_type, 0);
    return OK;
}
*/

OpenGLTextureBuffer::OpenGLTextureBuffer(Farg<TextureFormat> inFormat,
    Farg<SamplerState> inSampler,
    const FileData* inData):
        mFormat{inFormat}, mSampler{inSampler}, mStatus{OK}
{
    GLenum gl_type{s_GLType(mFormat.type)};
    GLenum gl_format{s_GLFormat(mFormat.data_format)};
    GLint  gl_internal_format{s_GLInternalFormat(mFormat.data_format)};
    unsigned char* image_data{nullptr};

    glGenTextures(1, &mBufferID);
    glBindTexture(gl_type, mBufferID);

    if(inData)
    {
        stbi_set_flip_vertically_on_load(true);
        int channels{};
        image_data = stbi_load_from_memory(inData->Data(),
            inData->Size(),
            &mFormat.width,
            &mFormat.height,
            &channels,
            STBI_rgb);
        if(!image_data)
        {
            print_warning("STBI failed to load image data; texture buffer {} will be empty", mBufferID);
            mStatus = ERR_DATA_LOAD;
        }
    }

    switch(gl_type)
    {
#pragma message("TODO: implement all texture types and image format types")
    case GL_TEXTURE_2D:
        glTexImage2D(GL_TEXTURE_2D,
            0,
            gl_internal_format,
            mFormat.width,
            mFormat.height,
            0,
            gl_format,
            GL_UNSIGNED_BYTE,
            image_data);
        print_debug("glTexImage2D(GL_TEXTURE_2D, 0, {}, {}, {}, 0, {}, GL_UNSIGNED_BYTE, image_data",
            gl_internal_format,
            mFormat.width,
            mFormat.height,
            gl_format);
        break;
    default:
        print_warning("The requested OpenGL texture format is unimplemented, sorry");
        mStatus = UNIMPLEMENTED;
        break;
    }
    if(mSampler.mip_filter_mag != SAMPLER_FILTER_NONE or mSampler.mip_filter_min != SAMPLER_FILTER_NONE)
        { glGenerateTextureMipmap(mBufferID); }
    glTextureParameteri(mBufferID, GL_TEXTURE_WRAP_S, s_GLWrap(mSampler.repeat_u));
    glTextureParameteri(mBufferID, GL_TEXTURE_WRAP_T, s_GLWrap(mSampler.repeat_v));
    glTextureParameteri(mBufferID, GL_TEXTURE_WRAP_R, s_GLWrap(mSampler.repeat_w));
    glTextureParameterf(mBufferID, GL_TEXTURE_MAX_ANISOTROPY, mSampler.use_anisotropy * mSampler.anisotropy_max);
    glTextureParameteri(mBufferID, GL_TEXTURE_MIN_FILTER, s_GLFilter(mSampler.min_filter, mSampler.mip_filter_min));
    glTextureParameteri(mBufferID, GL_TEXTURE_MAG_FILTER, s_GLFilter(mSampler.mag_filter, mSampler.mip_filter_mag));
    glBindTexture(gl_type, 0);
    stbi_image_free(image_data);
}

OpenGLTextureBuffer::~OpenGLTextureBuffer()
{ glDeleteTextures(1, &mBufferID); }

void OpenGLTextureBuffer::Bind()
{
    Unbind();
    glBindTexture(s_GLType(mFormat.type), mBufferID);
    mIsBound = true;
}

void OpenGLTextureBuffer::Bind(uint inTextureUnit)
{ glBindTextureUnit(inTextureUnit, mBufferID); }

void OpenGLTextureBuffer::Unbind()
{
    if(mIsBound)
    {
        glBindTextureUnit(s_GLType(mFormat.type), 0);
        mIsBound = false;
    }
}

Error OpenGLTextureBuffer::Status() const
{ return mStatus; }

uint OpenGLTextureBuffer::ID() const
{ return mBufferID; }

Farg<TextureFormat> OpenGLTextureBuffer::Format() const
{ return mFormat; }

Farg<SamplerState> OpenGLTextureBuffer::Sampler() const
{ return mSampler; }
