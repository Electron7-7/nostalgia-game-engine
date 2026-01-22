#include "application.hpp"

#include <cassert>

IApplication* sInstance{nullptr};

IApplication* Application()
{ return sInstance; }

Unique<IWindow>& MainWindow()
{ return Application()->GetWindow(); }

IApplication::IApplication()
{
    assert(!sInstance && "Only one Application instance allowed!");
    PRINT_PRETTY_CONSTRUCTOR;
    sInstance = this;
}

IApplication::~IApplication()
{ PRINT_PRETTY_DESTRUCTOR; sInstance = nullptr; }

Unique<IWindow>& IApplication::GetWindow()
{ return mMainWindow; }
