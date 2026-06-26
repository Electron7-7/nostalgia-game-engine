#ifndef OPENGL_ENUM_CONVERSIONS_H
#define OPENGL_ENUM_CONVERSIONS_H

#include "fwd/rendering.hpp"

namespace Convert
{
    uint GL_DataFormat(DataFormat, bool inGetInternal = false);
    uint GL_BufferAttachment(BufferAttachment);
    uint GL_BufferAttachment(DataFormat);
    uint GL_TextureType(TextureType);
    uint GL_SamplerRepeat(SamplerRepeat);
    uint GL_SamplerFilter(SamplerFilter, SamplerFilter);
    void GL_SamplerFilterMin(uint, SamplerFilter&, SamplerFilter&);
    void GL_SamplerFilterMag(uint, SamplerFilter&);
    void GL_SamplerRepeat(uint, SamplerRepeat&);
}

#endif // OPENGL_ENUM_CONVERSIONS_H
