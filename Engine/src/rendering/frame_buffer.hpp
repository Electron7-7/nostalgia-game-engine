#ifdef FWD_DCL
class FrameBuffer;
#elif !defined FRAME_BUFFER_H
#define FRAME_BUFFER_H

#define FWD_DCL
#   include "filesystem/file_data.hpp"
#undef  FWD_DCL

#include "core/id.hpp"
#include "core/error.hpp"
#include "core/smart_pointers.hpp"
#include "math/containers.hpp"

#define FWD_DCL
#   include "rendering/texture_buffer.hpp"
#undef  FWD_DCL

class FrameBuffer
{
public:
    struct IDs
    {
        static constexpr uint Default{0};
    };

    virtual ~FrameBuffer() = default;

    virtual uint Generate() = 0;
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual uint GetID() const = 0;
    virtual Farg<Scale2D> GetScale() const = 0;
    virtual Error SetOutputTexture(Shared<TextureBuffer> inTextureBuffer, ID inID = ID::Invalid) = 0;
    virtual ID GetOutputTextureID() const = 0;
    // virtual Error SetOutputRenderBuffer() = 0;
    // virtual ID GetOutputRenderBufferID() const = 0;

    static Shared<FrameBuffer> Create();
};

#endif // FRAME_BUFFER_H
