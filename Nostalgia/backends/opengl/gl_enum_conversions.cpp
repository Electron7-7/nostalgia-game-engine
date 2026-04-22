#include "./gl_enum_conversions.hpp"
#include "rendering/texture_buffer.hpp"
#include "thirdparty/glad/glad.h"

uint Convert::GL_DataFormat(DataFormat inFormat, bool inGetInternal)
{
    switch(inFormat)
    {
    case DATA_FORMAT_RED:
        if(inGetInternal) { return GL_R8; }
        else              { return GL_RED;  }
    case DATA_FORMAT_RGB:
        if(inGetInternal) { return GL_RGB8; }
        else              { return GL_RGB;  }
    case DATA_FORMAT_RGBA:
    case DATA_FORMAT_RGBA8:
        if(inGetInternal) { return GL_RGBA8; }
        else              { return GL_RGBA;  }
    case DATA_FORMAT_SRGB:
        if(inGetInternal) { return GL_SRGB8; }
        else              { return GL_RGB;   }
    case DATA_FORMAT_SRGB_ALPHA:
        if(inGetInternal) { return GL_SRGB8_ALPHA8; }
        else              { return GL_RGBA;         }
    case DATA_FORMAT_DEPTH:
    case DATA_FORMAT_DEPTH32:
        if(inGetInternal) { return GL_DEPTH_COMPONENT32; }
        else              { return GL_DEPTH_COMPONENT;   }
    case DATA_FORMAT_NONE:
    default:
        print_error("Invalid data format! Returning `GL_NONE` (this could cause a crash!)");
        return GL_NONE;
    }
}

uint Convert::GL_BufferAttachment(BufferAttachment inAttachment)
{
    switch(inAttachment)
    {
    case ATTACHMENT_COLOR:
#pragma message("TODO: implement support for more than just the first color attachment")
        return GL_COLOR_ATTACHMENT0;
    case ATTACHMENT_DEPTH:
        return GL_DEPTH_ATTACHMENT;
    case ATTACHMENT_STENCIL:
        return GL_STENCIL_ATTACHMENT;
    default:
        print_error("Invalid buffer attachment! Returning `GL_NONE` (this could cause a crash!)");
        return GL_NONE;
    }
}

uint Convert::GL_BufferAttachment(DataFormat inFormat)
{
    switch(inFormat)
    {
    case DATA_FORMAT_RED:
    case DATA_FORMAT_RGB:
    case DATA_FORMAT_RGBA:
    case DATA_FORMAT_RGBA8:
    case DATA_FORMAT_SRGB:
    case DATA_FORMAT_SRGB_ALPHA:
#pragma message("TODO: implement support for more than just the first color attachment")
        return GL_COLOR_ATTACHMENT0;
    case DATA_FORMAT_DEPTH:
    case DATA_FORMAT_DEPTH32:
        return GL_DEPTH_ATTACHMENT;
    case DATA_FORMAT_NONE:
    default:
        print_error("Invalid data format! Returning `GL_NONE` (this could cause a crash!)");
        return GL_NONE;
    }
}

uint Convert::GL_TextureType(TextureType inType)
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

uint Convert::GL_SamplerRepeat(SamplerRepeat inRepeat)
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

uint Convert::GL_SamplerFilter(SamplerFilter inFilter, SamplerFilter inMipFilter)
{
    switch(inFilter)
    {
    case SAMPLER_FILTER_NONE:
        return GL_NONE;
    case SAMPLER_FILTER_LINEAR:
        switch(inMipFilter)
        {
        case SAMPLER_FILTER_NONE:
            return GL_LINEAR;
        case SAMPLER_FILTER_LINEAR:
            return GL_LINEAR_MIPMAP_LINEAR;
        case SAMPLER_FILTER_NEAREST:
            return GL_LINEAR_MIPMAP_NEAREST;

        }
    case SAMPLER_FILTER_NEAREST:
        switch(inMipFilter)
        {
        case SAMPLER_FILTER_NONE:
            return GL_NEAREST;
        case SAMPLER_FILTER_LINEAR:
            return GL_NEAREST_MIPMAP_LINEAR;
        case SAMPLER_FILTER_NEAREST:
            return GL_NEAREST_MIPMAP_NEAREST;
        }
    }
}
