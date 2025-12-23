#ifndef RENDERER_COMMON_H
#define RENDERER_COMMON_H

// Heavily plagiarizing Godot's rendering device systems

#include <sys/types.h>

// NOT YET IMPLEMENTED:
// enum VertexFrequency - when I implement instanced rendering(?)
// static const uint ATTACHMENT_UNUSED{-1} - used by other objects when not attaching to a framebuffer
// enum UniformType - I'm assuming this is used for when uniforms are sent as objects instead of single elements/scalars

enum DataFormat
{
    DATA_FORMAT_NONE,
};

enum SamplerFilter
{
    SAMPLER_FILTER_NEAREST,
    SAMPLER_FILTER_LINEAR,
};

enum SamplerRepeatMode
{
    SAMPLER_REPEAT_MODE_REPEAT,
    SAMPLER_REPEAT_MODE_MIRRORED_REPEAT,
    SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE,
    SAMPLER_REPEAT_MODE_CLAMP_TO_BORDER,
    SAMPLER_REPEAT_MODE_MIRROR_CLAMP_TO_EDGE,
};

struct SamplerState
{
    SamplerFilter mag_filter{SAMPLER_FILTER_NEAREST};
    SamplerFilter min_filter{SAMPLER_FILTER_NEAREST};
    SamplerFilter mip_filter{SAMPLER_FILTER_NEAREST};
    SamplerRepeatMode repeat_u{SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE};
    SamplerRepeatMode repeat_v{SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE};
    SamplerRepeatMode repeat_w{SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE};
    bool use_anisotropy{false};
    float anisotropy_max{1.0f};
};

enum ShaderLanguage
{
    SHADER_LANGUAGE_GLSL,
    // SHADER_LANGUAGE_HLSL,
};

enum RenderPrimitive // used like GL_TRIANGLES or GL_POINTS
{
    RENDER_PRIMITIVE_TRIANGLES,
    RENDER_PRIMITIVE_POINTS,
    RENDER_PRIMITIVE_LINES,
};

enum PolygonCullMode
{
    POLYGON_CULL_MODE_DISABLED,
    POLYGON_CULL_MODE_FRONT,
    POLYGON_CULL_MODE_BACK,
};

struct VertexAttribute
{
    uint location{0}; // layout(location = N)
    uint offset{0};
    uint stride{0};
    DataFormat format{DataFormat::DATA_FORMAT_NONE};
};

#endif // RENDERER_COMMON_H
