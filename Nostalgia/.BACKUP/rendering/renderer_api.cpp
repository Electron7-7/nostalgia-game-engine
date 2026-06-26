#include "./renderer_api.hpp"
// Implementations
#include "backends/dummy_renderer_api.hpp"
#include "backends/opengl/gl_renderer_api.hpp"

#define LOCK_API LockGuard<RMutex> _lock{m_sInstanceMutex};

Unique<RendererAPI> RendererAPI::ms_pInstance{MakeUnique<DummyRendererAPI>()};

bool RendererAPI::ActivateInstance(GraphicsAPI inAPI)
{
    LockGuard<RMutex> _lock{m_sInstanceMutex};
    switch(inAPI)
    {
    default:
    case NONE:
        print_debug("Deactivating current instance");
        DeactivateInstance();
        ms_pInstance = MakeUnique<DummyRendererAPI>();
        m_sInstanceActive = false;
        break;
    case OPENGL:
        if(m_sInstanceActive)
            { return false; }
        print_debug("Activating OpenGLRendererAPI");
        ms_pInstance = MakeUnique<OpenGLRendererAPI>();
        m_sInstanceActive = true;
        break;
    }
    m_sType = inAPI;
    return true;
}

void RendererAPI::DeactivateInstance()
{
    LOCK_API
    ms_pInstance->Shutdown();
    ms_pInstance = MakeUnique<DummyRendererAPI>();
    m_sInstanceActive = false;
}

bool RendererAPI::HasActiveInstance()
{
    LOCK_API
    return m_sInstanceActive;
}

RendererAPI* RendererAPI::Get()
{
    LOCK_API
    return ms_pInstance.get();
}

RendererAPI::GraphicsAPI RendererAPI::CurrentAPI()
{
    LOCK_API
    return m_sType;
}

LockGuard<RMutex> RendererAPI::GetLock()
{ return LockGuard<RMutex>{m_sInstanceMutex}; }
