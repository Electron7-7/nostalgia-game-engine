#include "gl_texture_buffer.hpp"
#include "core/printing.hpp"
#include "filesystem/file_data.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb_image/stb_image.h"

#include <glad/glad.h>

static GLenum sTextureTypeToOpenGLType(TextureType inType)
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
        print_warning("Invalid texture type! (returning GL_TEXTURE_2D");
        return GL_TEXTURE_2D;
    }
}

OpenGLTextureBuffer::OpenGLTextureBuffer()
{}

OpenGLTextureBuffer::~OpenGLTextureBuffer()
{
    UnbindUnit();
    glDeleteTextures(1, &mTextureID);
}

void OpenGLTextureBuffer::Bind() const
{ glBindTexture(sTextureTypeToOpenGLType(mTextureFormat.type), mTextureID); }

void OpenGLTextureBuffer::Unbind() const
{ glBindTexture(sTextureTypeToOpenGLType(mTextureFormat.type), 0); }

void OpenGLTextureBuffer::BindUnit(uint inTextureUnit)
{
    mTextureUnit = inTextureUnit;
    glBindTextureUnit(mTextureUnit, mTextureID);
}

void OpenGLTextureBuffer::UnbindUnit() const
{ glBindTextureUnit(mTextureUnit, 0); }

uint OpenGLTextureBuffer::GetID() const
{ return mTextureID; }

Farg<TextureFormat> OpenGLTextureBuffer::GetFormat() const
{ return mTextureFormat; }

Error OpenGLTextureBuffer::GenerateTexture(Farg<TextureFormat> inTextureFormat)
{
    GLenum gl_type{sTextureTypeToOpenGLType(mTextureFormat.type)};
    glCreateTextures(gl_type, 1, &mTextureID);
    glBindTexture(gl_type, mTextureID);
#pragma message("FIXME: I don't know how to properly use 'glTextureSubImage2D' yet")
    switch(gl_type)
    {
#pragma message("TODO: implement all texture types and image format types")
    case GL_TEXTURE_2D:
        glTexImage2D(gl_type,
            0,
            GL_RGB,
            mTextureFormat.width,
            mTextureFormat.height,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            nullptr);
        break;
    default:
        glDeleteTextures(1, &mTextureID);
        print_error("Unimplemented OpenGL texture format");
        return UNIMPLEMENTED;
    }
    glTexParameteri(gl_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(gl_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    return OK;
}

Error OpenGLTextureBuffer::GenerateTexture(Farg<FileData> inData, Farg<TextureFormat> inTextureFormat)
{
#pragma message("TODO: Only flip images that need to be flipped")
    stbi_set_flip_vertically_on_load(true);

    GLenum gl_type{sTextureTypeToOpenGLType(mTextureFormat.type)};

    glCreateTextures(gl_type, 1, &mTextureID);

    unsigned char* image_data{nullptr};

    if(inData.Status() == DataStatus::SUCCESSFUL)
    {
        image_data = stbi_load_from_memory(inData.Data(),
            inData.Size(),
            &mTextureFormat.width,
            &mTextureFormat.height,
            &mTextureFormat.channels,
            STBI_rgb);

        if(!image_data)
        {
            glDeleteTextures(1, &mTextureID);
            print_error("STBI failed to load image");
            return ERR_FILE;
        }
    }

#pragma message("FIXME: I don't know how to properly use 'glTextureSubImage2D' yet")
    glBindTexture(gl_type, mTextureID);
    switch(gl_type)
    {
#pragma message("TODO: implement all texture types and image format types")
    case GL_TEXTURE_2D:
        glTexImage2D(gl_type,
            0,
            GL_SRGB_ALPHA,
            mTextureFormat.width,
            mTextureFormat.height,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            image_data);
        break;
    default:
        stbi_image_free(image_data);
        glDeleteTextures(1, &mTextureID);
        print_error("Unimplemented OpenGL texture format");
        return UNIMPLEMENTED;
    }
#pragma message("TODO: implement different texture filtering, wrap, and anisotropy types")
    glGenerateTextureMipmap(mTextureID);
    glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTextureParameterf(mTextureID, GL_TEXTURE_MAX_ANISOTROPY, 16);
    glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(image_data);
    return OK;
}
