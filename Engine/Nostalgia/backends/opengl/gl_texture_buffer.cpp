#include "gl_texture_buffer.hpp"
#include "thirdparty/glad/glad.h"

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

static GLint s_GLFormat(DataFormat inFormat, bool getInternal = false)
{
    switch(inFormat)
    {
    case DATA_FORMAT_RED:
        return GL_RED;
    case DATA_FORMAT_RGB:
        if(getInternal) { return GL_RGB8; }
        else            { return GL_RGB;  }
    case DATA_FORMAT_SRGB:
        if(getInternal) { return GL_SRGB8; }
        else            { return GL_RGB;   }
    case DATA_FORMAT_RGBA:
    case DATA_FORMAT_RGBA8:
        if(getInternal) { return GL_RGBA8; }
        else            { return GL_RGBA;  }
    case DATA_FORMAT_SRGB_ALPHA:
        if(getInternal) { return GL_SRGB8_ALPHA8; }
        else            { return GL_RGBA;         }
    case DATA_FORMAT_NONE:
    default:
        print_error("Invalid data format! Returning `GL_NONE` (this could cause a crash!)");
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

Error OpenGLTextureBuffer::Load(InputData inData, Farg<TextureFormat> inFormat)
{
    GLenum gl_type{s_GLType(mType = inFormat.type)};
    GLenum gl_target{(gl_type == GL_TEXTURE_CUBE_MAP)
        ? GL_TEXTURE_CUBE_MAP_POSITIVE_X + mCubemapOffsetIterator++
        : gl_type};

    glBindTexture(gl_type, mBufferID);
    if(inFormat.data_format == DATA_FORMAT_RED) { glPixelStorei(GL_UNPACK_ALIGNMENT, 1); }
    glTexImage2D(gl_target,
        0,
        s_GLFormat(inFormat.data_format, true),
        inFormat.width,
        inFormat.height,
        0,
        s_GLFormat(inFormat.data_format),
        GL_UNSIGNED_BYTE,
        inData);

    glBindTexture(gl_type, 0);
    return mStatus = OK;
}

Error OpenGLTextureBuffer::GenerateMipMaps()
{
    glBindTexture(s_GLType(mType), mBufferID);
    glGenerateTextureMipmap(mBufferID);
    glBindTexture(s_GLType(mType), 0);
    return OK;
}

Error OpenGLTextureBuffer::SetSamplerState(Farg<SamplerState> inSamplerState) const
{
    glBindTexture(s_GLType(mType), mBufferID);
    if(inSamplerState.use_anisotropy)
        { glTextureParameterf(mBufferID, GL_TEXTURE_MAX_ANISOTROPY, inSamplerState.anisotropy_max); }
    glTextureParameteri(mBufferID, GL_TEXTURE_MIN_FILTER, s_GLFilter(inSamplerState.min_filter, inSamplerState.mip_filter_min));
    glTextureParameteri(mBufferID, GL_TEXTURE_MAG_FILTER, s_GLFilter(inSamplerState.mag_filter, SAMPLER_FILTER_NONE));
    glTextureParameteri(mBufferID, GL_TEXTURE_WRAP_S, s_GLWrap(inSamplerState.repeat_u));
    glTextureParameteri(mBufferID, GL_TEXTURE_WRAP_T, s_GLWrap(inSamplerState.repeat_v));
    glTextureParameteri(mBufferID, GL_TEXTURE_WRAP_R, s_GLWrap(inSamplerState.repeat_w));
    glBindTexture(s_GLType(mType), 0);

    if(mType == TEXTURE_TYPE_NONE)
        { return FAILED; }
    else if(!mStatus)
        { return mStatus; }
    return OK;
}

OpenGLTextureBuffer::OpenGLTextureBuffer()
{ glGenTextures(1, &mBufferID); }

OpenGLTextureBuffer::~OpenGLTextureBuffer()
{ glDeleteTextures(1, &mBufferID); }

Error OpenGLTextureBuffer::Status() const
{ return mStatus; }

uint OpenGLTextureBuffer::ID() const
{ return mBufferID; }

TextureType OpenGLTextureBuffer::Type() const
{ return mType; }
