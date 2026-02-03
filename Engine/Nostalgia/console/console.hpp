#ifndef CONSOLE_H
#define CONSOLE_H

#include <Nostalgia/fwd/core.hpp>

typedef Error (*ConsoleCommandCallback_f)(Sarg inCommand);

class DebugConsole
{
public:
    static Shared<DebugConsole> Instance();
    static Shared<DebugConsole> Create();

    virtual void  SetConsoleCommandCallback(ConsoleCommandCallback_f) = 0;
    virtual Error AddConsoleVariable(Sarg inName);
    virtual Error RemoveConsoleVariable(Sarg inName);
    virtual Error SetConsoleVariable(Sarg inName, Sarg inValue);

private:
    static Shared<DebugConsole> m_sInstance;
};

#endif // CONSOLE_H
