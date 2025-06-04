#ifndef THE_TOYMAKER_H
#define THE_TOYMAKER_H

#include "application.hpp"
#include "common/macros.hpp"

FORWARD_DECLARE(class TheToyMaker;)
FORWARD_DECLARE(extern TheToyMaker* global_App;)

class TheToyMaker : public _Application
{
    typedef _Application BaseClass;

public:
    virtual bool Create();
    virtual bool PreInit();
    virtual int Main();
    virtual void Shutdown();
    virtual void PostShutdown();
    virtual const char* GetAppName() { return "TheToyMaker"; };
};

#endif // THE_TOYMAKER_H