#ifndef RENDERER_COMMON_H
#define RENDERER_COMMON_H

// Heavily plagiarizing Godot's rendering device systems

// NOT YET IMPLEMENTED:
// enum VertexFrequency - when I implement instanced rendering(?)
// static const uint ATTACHMENT_UNUSED{-1} - used by other objects when not attaching to a framebuffer
// enum UniformType - I'm assuming this is used for when uniforms are sent as objects instead of single elements/scalars

enum DataFormat
{
    DATA_FORMAT_NONE,
    DATA_FORMAT_RED,
    DATA_FORMAT_RGB,
    DATA_FORMAT_RGBA,
    DATA_FORMAT_RGBA8,
    DATA_FORMAT_SRGB,
    DATA_FORMAT_SRGB_ALPHA,
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
