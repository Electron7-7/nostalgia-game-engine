#ifndef RENDERING_FWD_H
#define RENDERING_FWD_H

enum DataFormat       : int;
enum BufferAttachment : int;

class FrameBuffer;

class RenderBuffer;

class GraphicsContext;

class IBuffer;
class VertexBuffer;
class IndexBuffer;

class RendererAPI;
class RenderCommand;

class Shader;
namespace Shaders
{
    extern constinit const uint BlinnPhong;
    extern constinit const uint Fullbright;
    extern constinit const uint SkyBox;
    extern constinit const uint Fonts;
}

enum TextureType   : int;
enum SamplerFilter : int;
enum SamplerRepeat : int;
struct SamplerState;
struct TextureFormat;
class TextureBuffer;

class VertexArray;

class Environment;

#endif // RENDERING_FWD_H
