#ifndef APPLICATION_H
#define APPLICATION_H

#include "common/macros.hpp"

FORWARD_DECLARE(struct GLFWwindow;)

/*
Big note on _Application:
---------------------------------------
    I have no fucking idea what I'm doing and I'm just winging it here. After digging through the source code of Source, I
    can't for the fucking life of me figure out where, when, or how `IApplication` is declared, defined or even fucking written down.
    That means I'm just fucking winging the whole entire Application system.
    Time to see if I know what I'm doing and/or if I've learned anything at all...
*/

class _Application
{
public:
    // Applications are expected to implement these methods
    virtual bool Create()  { return true; }
    virtual bool PreInit() { return true; }
    virtual int Main()     { return 0;    }
    virtual void Shutdown() {}
    virtual void PostShutdown() {}
    virtual const char* GetAppName() { return "_Application (you really shouldn't fucking see this!!!)"; }
};
#endif // APPLICATION_H
