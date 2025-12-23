#include "implementor.hpp"
#include "solution.hpp"

UI_Implementor::~UI_Implementor() = default;

// Non-static functions
UI_Implementor::InstanceState UI_Implementor::GetState() const
{ return mState; }

// Static functions
void UI_Implementor::AttachAll()
{ InvokeMethod(&UI_Implementor::Attach); }

void UI_Implementor::DetachAll()
{ InvokeMethod(&UI_Implementor::Detach); }

void UI_Implementor::BeginAll()
{ InvokeMethod(&UI_Implementor::Begin); }

void UI_Implementor::EndAll()
{ InvokeMethod(&UI_Implementor::End); }

void UI_Implementor::InvokeMethod(ImplementorFunctionPtr inFunctionPtr)
{
    for(auto& [index, instance] : m_sInstances)
        { if(!instance) { continue; } (instance.get()->*inFunctionPtr)(); }
}

void UI_Implementor::InvokeMethod(SolutionFunction_ptr inFunctionPtr)
{
    for(auto& [index, instance] : m_sInstances)
    {
        if(!instance->mAttached)
            { continue; }
        for(auto& [index, object] : instance->mObjects)
            { if(!object) { continue; } (object.get()->*inFunctionPtr)(); }
    }
}

void UI_Implementor::InvokeMethod(SolutionInit_ptr inInitPtr)
{
    for(auto& [index, instance] : m_sInstances)
    {
        if(!instance->mAttached)
            { continue; }
        for(auto& [index, object] : instance->mObjects)
            { if(!object) { continue; } (object.get()->*inInitPtr)(); }
    }
}

void UI_Implementor::InvokeMethod(SolutionInputEventFunction_ptr inFunctionPtr, Farg<InputEvent> inEvent)
{
    for(auto& [index, instance] : m_sInstances)
    {
        if(!instance->mAttached)
            { continue; }
        for(auto& [index, object] : instance->mObjects)
            { if(!object) { continue; } (object.get()->*inFunctionPtr)(inEvent); }
    }
}
