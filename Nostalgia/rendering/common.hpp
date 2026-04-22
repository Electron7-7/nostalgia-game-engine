#ifndef RENDERER_COMMON_H
#define RENDERER_COMMON_H

// Heavily plagiarizing Godot's rendering device systems

// NOT YET IMPLEMENTED:
// enum VertexFrequency - when I implement instanced rendering(?)
// static const uint ATTACHMENT_UNUSED{-1} - used by other objects when not attaching to a framebuffer
// enum UniformType - I'm assuming this is used for when uniforms are sent as objects instead of single elements/scalars

enum DataFormat : int
{
    DATA_FORMAT_NONE,
    DATA_FORMAT_RED,
    DATA_FORMAT_RGB,
    DATA_FORMAT_RGBA,
    DATA_FORMAT_RGBA8,
    DATA_FORMAT_SRGB,
    DATA_FORMAT_SRGB_ALPHA,
    DATA_FORMAT_DEPTH,   // e.g. GL_DEPTH_COMPONENT
    DATA_FORMAT_DEPTH32, // e.g. GL_DEPTH_COMPONENT32
};

enum BufferAttachment : int
{
    ATTACHMENT_COLOR,
    ATTACHMENT_DEPTH,
    ATTACHMENT_STENCIL,
    // ATTACHMENT_DEPTH_STENCIL,
};

#endif // RENDERER_COMMON_H
