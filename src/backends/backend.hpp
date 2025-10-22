#ifndef BACKEND_H
#define BACKEND_H

#include "common/ids.hpp"

namespace BackendIDs
{
    constexpr ID Invalid { ID::Invalid };
    constexpr ID wGLFW   { "glfw"      };
    constexpr ID gOpenGL { "opengl"    };
}

class _Backend
{
public:
    // `Init` should only be called by `BackendManager::Init`. This ensures it's only called once.
    virtual bool Init() = 0;
    // `Shutdown` should only be called by `BackendManager::Shutdown`. This ensures it's only called once.
    virtual void Shutdown() = 0;
    virtual const ID& GetID() const = 0;

    // 'ImGui' methods should only be called by `BackendManager` so it can confirm that ImGui is initialized.
    virtual bool InitImGui() = 0;
    virtual void ShutdownImGui() = 0;
    virtual void ImGuiNewFrame() = 0;

protected:
    virtual ~_Backend() = default;
};

#endif // BACKEND_H
