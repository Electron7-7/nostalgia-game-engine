#include "./gl_texture_buffer.hpp"
#include "./gl_enum_conversions.hpp"
#include "managers/render_manager.hpp"
#include "thirdparty/glad/glad.h"

#define ASSERT_API if(not g_pRenderManager->IsAPIActive()) { return; }

OpenGLTextureBuffer::OpenGLTextureBuffer()
{
    ASSERT_API
    glGenTextures(1, &mBufferID);
}

OpenGLTextureBuffer::~OpenGLTextureBuffer()
{
    ASSERT_API
    glDeleteTextures(1, &mBufferID);
}

void OpenGLTextureBuffer::GenerateMipMaps()
{
    ASSERT_API
    glBindTexture(Convert::GL_TextureType(mType), mBufferID);
    glGenerateTextureMipmap(mBufferID);
    glBindTexture(Convert::GL_TextureType(mType), 0);
}

void OpenGLTextureBuffer::SetSamplerState(Farg<SamplerState> inSamplerState) const
{
    ASSERT_API
    glBindTexture(Convert::GL_TextureType(mType), mBufferID);
    if(inSamplerState.use_anisotropy)
        { glTextureParameterf(mBufferID, GL_TEXTURE_MAX_ANISOTROPY, inSamplerState.anisotropy_max); }
    glTextureParameteri(mBufferID, GL_TEXTURE_MIN_FILTER, Convert::GL_SamplerFilter(inSamplerState.min_filter, inSamplerState.mip_filter_min));
    glTextureParameteri(mBufferID, GL_TEXTURE_MAG_FILTER, Convert::GL_SamplerFilter(inSamplerState.mag_filter, SAMPLER_FILTER_NONE));
    glTextureParameteri(mBufferID, GL_TEXTURE_WRAP_S, Convert::GL_SamplerRepeat(inSamplerState.repeat_u));
    glTextureParameteri(mBufferID, GL_TEXTURE_WRAP_T, Convert::GL_SamplerRepeat(inSamplerState.repeat_v));
    glTextureParameteri(mBufferID, GL_TEXTURE_WRAP_R, Convert::GL_SamplerRepeat(inSamplerState.repeat_w));
    glBindTexture(Convert::GL_TextureType(mType), 0);
}

void OpenGLTextureBuffer::Load(InputData inData, Farg<TextureFormat> inFormat, int inLayer)
{
    ASSERT_API
    GLenum gl_type{Convert::GL_TextureType(mType = inFormat.type)};
    GLenum gl_target{(gl_type == GL_TEXTURE_CUBE_MAP)
        ? GL_TEXTURE_CUBE_MAP_POSITIVE_X + inLayer
        : gl_type};

    glBindTexture(gl_type, mBufferID);
    if(inFormat.data_format == DATA_FORMAT_RED)
        { glPixelStorei(GL_UNPACK_ALIGNMENT, 1); }
    glTexImage2D(gl_target,
        0,
        Convert::GL_DataFormat(inFormat.data_format, true),
        inFormat.width,
        inFormat.height,
        0,
        Convert::GL_DataFormat(inFormat.data_format),
        GL_UNSIGNED_BYTE,
        inData);
    glBindTexture(gl_type, 0);
}

TextureType OpenGLTextureBuffer::Type() const
{ return mType; }

uint OpenGLTextureBuffer::ID() const
{ return mBufferID; }

Shared<Image> OpenGLTextureBuffer::GetImage(int inMipmapLevel) const
{
    if(not g_pRenderManager->IsAPIActive())
        { return Image::CreateEmpty(1, 1); }
    GLint _width, _height;
    glGetTextureLevelParameteriv(mBufferID, inMipmapLevel, GL_TEXTURE_WIDTH, &_width);
    glGetTextureLevelParameteriv(mBufferID, inMipmapLevel, GL_TEXTURE_HEIGHT, &_height);
    std::vector<uchar> _data(4 * _width * _height);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glGetTextureImage(mBufferID,
        inMipmapLevel,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        _data.size(),
        _data.data());
    return Image::CreateFromData(_data.data(), _data.size(), _width, _height, 4, false);
}
