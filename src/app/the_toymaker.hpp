#ifndef THE_TOYMAKER_H
#define THE_TOYMAKER_H

#include "application.hpp"

class TheToymaker : public _Application
{
    typedef _Application BaseClass;

public:
    virtual bool Create();
    virtual bool PreInit();
    virtual int Main();
    virtual void Shutdown();
    virtual void PostShutdown();
    virtual const char* GetAppName() { return "The Toymaker"; };
};

extern TheToymaker* global_EditorApp;

#endif // THE_TOYMAKER_H
