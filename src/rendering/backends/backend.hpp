#ifndef BACKEND_H
#define BACKEND_H

typedef int BackendID;

struct gBackendIDs
{
    static constexpr BackendID Invalid = -1;
    static constexpr BackendID gOpenGL =  0;
    static constexpr BackendID wGLFW   =  0;
};

class _Backend
{
public:
    virtual bool Init()       = 0;
    virtual void Shutdown()   = 0;
    virtual BackendID GetID() = 0;

protected:
    virtual ~_Backend() = default;
    bool m_IsInitialized = false;
};

#endif // BACKEND_H
