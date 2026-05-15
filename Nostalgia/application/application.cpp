#include "application.hpp"

IApplication* sInstance{nullptr};

IApplication* Application()
{ return sInstance; }

IApplication::IApplication()
{
    assert(!sInstance && "Only one Application instance allowed!");
    PRINT_PRETTY_CONSTRUCTOR;
    sInstance = this;
}

IApplication::~IApplication()
{
    PRINT_PRETTY_DESTRUCTOR;
    sInstance = nullptr;
}
