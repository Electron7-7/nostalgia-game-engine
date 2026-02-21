#ifndef RENDERING_FWD_H
#define RENDERING_FWD_H

class FrameBuffer;

class IGraphicsContext;

class IBuffer;
class VertexBuffer;
class IndexBuffer;

enum class GraphicsAPI;
class RendererAPI;

class Shader;
namespace Shaders
{
    extern constinit const uint BlinnPhong;
    extern constinit const uint Fullbright;
    extern constinit const uint SkyBox;
    extern constinit const uint Fonts;
}

enum TextureType : int;
enum SamplerFilter : int;
enum SamplerRepeat : int;
struct SamplerState;
struct TextureFormat;
class TextureBuffer;

class VertexArray;

class Environment;

#endif // RENDERING_FWD_H
