#include "./gl_texture_buffer.hpp"
#include "./gl_enum_conversions.hpp"
#include "rendering/renderer_api.hpp"
#include "thirdparty/glad/glad.h"

#define ASSERT_API_RETURN(VALUE) if(not RendererAPI::HasActiveInstance()) { return VALUE; }
#define ASSERT_API ASSERT_API_RETURN()

OpenGLTextureBuffer::OpenGLTextureBuffer(Farg<TextureFormat> inFormat):
    mFormat{inFormat}
{
    ASSERT_API
    glCreateTextures(Convert::GL_TextureType(mFormat.type), 1, &mBufferID);
    GLsizei _height{mFormat.height}, _depth{mFormat.depth};
    switch(mFormat.type)
    {
    case TEXTURE_TYPE_1D:
        glTextureStorage1D(mBufferID,
            mFormat.mipmaps + 1,
            Convert::GL_DataFormat(mFormat.data_format, true),
            mFormat.width);
        break;
    case TEXTURE_TYPE_1D_ARRAY:
        _height = mFormat.array_layers;
        [[fallthrough]];
    case TEXTURE_TYPE_CUBE:
    case TEXTURE_TYPE_2D:
        glTextureStorage2D(mBufferID,
            mFormat.mipmaps + 1,
            Convert::GL_DataFormat(mFormat.data_format, true),
            mFormat.width,
            _height);
        break;
    case TEXTURE_TYPE_2D_ARRAY:
        _depth = mFormat.array_layers;
        [[fallthrough]];
    case TEXTURE_TYPE_3D:
        glTextureStorage3D(mBufferID,
            mFormat.mipmaps + 1,
            Convert::GL_DataFormat(mFormat.data_format, true),
            mFormat.width,
            _height,
            _depth);
        break;
    }
}

OpenGLTextureBuffer::~OpenGLTextureBuffer()
{
    ASSERT_API
    glDeleteTextures(1, &mBufferID);
}

void OpenGLTextureBuffer::GenerateMipmaps()
{
    ASSERT_API
    if(mFormat.mipmaps)
        { glGenerateTextureMipmap(mBufferID); }
}

void OpenGLTextureBuffer::SetSamplerState(Farg<SamplerState> inSamplerState)
{
    ASSERT_API
    if(inSamplerState.use_anisotropy)
        { glTextureParameterf(mBufferID, GL_TEXTURE_MAX_ANISOTROPY, inSamplerState.anisotropy_max); }
    glTextureParameteri(mBufferID,
        GL_TEXTURE_MIN_FILTER,
        Convert::GL_SamplerFilter(inSamplerState.min_filter, inSamplerState.mip_filter_min));
    glTextureParameteri(mBufferID,
        GL_TEXTURE_MAG_FILTER,
        Convert::GL_SamplerFilter(inSamplerState.mag_filter, SAMPLER_FILTER_NONE));
    glTextureParameteri(mBufferID,
        GL_TEXTURE_WRAP_S,
        Convert::GL_SamplerRepeat(inSamplerState.repeat_u));
    glTextureParameteri(mBufferID,
        GL_TEXTURE_WRAP_T,
        Convert::GL_SamplerRepeat(inSamplerState.repeat_v));
    glTextureParameteri(mBufferID,
        GL_TEXTURE_WRAP_R,
        Convert::GL_SamplerRepeat(inSamplerState.repeat_w));
    GenerateMipmaps();
}

void OpenGLTextureBuffer::SetData(Farg<TextureDataFormat> inFormat)
{
    ASSERT_API

    GLint _yoffset{inFormat.yoffset}, _zoffset{inFormat.zoffset},
        _width{(inFormat.width   == -1) ? mFormat.width  : inFormat.width},
        _height{(inFormat.height == -1) ? mFormat.height : inFormat.height},
        _depth{(inFormat.depth   == -1) ? mFormat.depth  : inFormat.depth};

    glPixelStorei(GL_UNPACK_ALIGNMENT, GetChannels(mFormat.data_format));

    switch(mFormat.type)
    {
    default:
    case TEXTURE_TYPE_1D:
        glTextureSubImage1D(mBufferID,
            inFormat.lod,
            inFormat.xoffset,
            _width,
            Convert::GL_DataFormat(mFormat.data_format, true),
            GL_UNSIGNED_BYTE,
            inFormat.data);
        break;
    case TEXTURE_TYPE_1D_ARRAY:
        _yoffset = inFormat.layer;
        [[fallthrough]];
    case TEXTURE_TYPE_2D:
        glTextureSubImage2D(mBufferID,
            inFormat.lod,
            inFormat.xoffset,
            _yoffset,
            _width,
            _height,
            Convert::GL_DataFormat(mFormat.data_format),
            GL_UNSIGNED_BYTE,
            inFormat.data);
        break;
    case TEXTURE_TYPE_CUBE:
    case TEXTURE_TYPE_2D_ARRAY:
        _zoffset = inFormat.layer;
        [[fallthrough]];
    case TEXTURE_TYPE_3D:
        glTextureSubImage3D(mBufferID,
            inFormat.lod,
            inFormat.xoffset,
            inFormat.yoffset,
            _zoffset,
            _width,
            _height,
            _depth,
            Convert::GL_DataFormat(mFormat.data_format),
            GL_UNSIGNED_BYTE,
            inFormat.data);
        break;
    }
}

uint OpenGLTextureBuffer::GetID()
{ return mBufferID; }

Farg<TextureFormat> OpenGLTextureBuffer::GetFormat()
{ return mFormat; }

void OpenGLTextureBuffer::GetSamplerState(SamplerState& outSamplerState)
{
    ASSERT_API
    GLint _max_anisotropy, _min_filter, _mag_filter, _texture_wrap_s, _texture_wrap_t, _texture_wrap_r;
    glGetTextureParameteriv(mBufferID, GL_TEXTURE_MAX_ANISOTROPY, &_max_anisotropy);
    glGetTextureParameteriv(mBufferID, GL_TEXTURE_MIN_FILTER, &_min_filter);
    glGetTextureParameteriv(mBufferID, GL_TEXTURE_MAG_FILTER, &_mag_filter);
    glGetTextureParameteriv(mBufferID, GL_TEXTURE_WRAP_S, &_texture_wrap_s);
    glGetTextureParameteriv(mBufferID, GL_TEXTURE_WRAP_T, &_texture_wrap_t);
    glGetTextureParameteriv(mBufferID, GL_TEXTURE_WRAP_R, &_texture_wrap_r);

    outSamplerState.anisotropy_max = _max_anisotropy;
    outSamplerState.use_anisotropy = _max_anisotropy != 0;
    Convert::GL_SamplerFilterMin(_min_filter, outSamplerState.min_filter, outSamplerState.mip_filter_min);
    Convert::GL_SamplerFilterMag(_mag_filter, outSamplerState.mag_filter);
    Convert::GL_SamplerRepeat(_texture_wrap_s, outSamplerState.repeat_u);
    Convert::GL_SamplerRepeat(_texture_wrap_t, outSamplerState.repeat_v);
    Convert::GL_SamplerRepeat(_texture_wrap_r, outSamplerState.repeat_w);
}

Shared<Image> OpenGLTextureBuffer::GetImage(int inLayer, int inMipmapLevel)
{
    ASSERT_API_RETURN(Image::CreateEmpty(mFormat.width, mFormat.height, mFormat.data_format));
    GLint _channels{GetChannels(mFormat.data_format)};
    std::vector<uchar> _data(_channels * mFormat.width * mFormat.height);
    glPixelStorei(GL_UNPACK_ALIGNMENT, _channels);
    glGetTextureImage(mBufferID,
        inMipmapLevel,
        Convert::GL_DataFormat(mFormat.data_format),
        GL_UNSIGNED_BYTE,
        _data.size(),
        _data.data());
    return Image::CreateFromData(_data.data(),
        _data.size(),
        mFormat.width,
        mFormat.height,
        _channels,
        false,
        mFormat.data_format);
}
