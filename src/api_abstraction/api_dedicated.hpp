#ifndef API_DEDICATED_H
#define API_DEDICATED_H

class API
{
public:
    virtual ~API() = default;

    virtual bool Init() = 0;
    virtual void Shutdown() = 0;
    virtual bool CreateMainWindow() = 0;

protected:
    bool is_initialized = false;
};

namespace GraphicsAPI
{
    extern API* OpenGL;
    // new API objects go here...
    extern API* Default;
}

#endif // API_DEDICATED_H