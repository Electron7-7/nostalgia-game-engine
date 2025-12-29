#include "implementor.hpp"
#include "solution.hpp"

UI_Implementor::Instances UI_Implementor::m_sInstances{};
std::recursive_mutex UI_Implementor::m_sInstancesMutex{};

UI_Implementor::UI_Implementor()  = default;
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
    const std::lock_guard<std::recursive_mutex> lock{m_sInstancesMutex};
    for(auto& [index, instance] : m_sInstances)
    {
        if(instance)
            { (instance.get()->*inFunctionPtr)(); }
    }
}

void UI_Implementor::InvokeMethod(SolutionFunction_ptr inFunctionPtr)
{
    const std::lock_guard<std::recursive_mutex> lock{m_sInstancesMutex};
    for(auto& [index, instance] : m_sInstances)
    {
        if(!instance->mAttached)
            { continue; }
        for(auto& [index, object] : instance->mObjects)
        {
            if(object)
                { (object.get()->*inFunctionPtr)(); }
        }
    }
}

void UI_Implementor::InvokeInput(InputEvent* inEvent)
{
    const std::lock_guard<std::recursive_mutex> lock{m_sInstancesMutex};
    for(auto& [index, instance] : m_sInstances)
    {
        if(!instance->mAttached)
            { continue; }
        for(auto& [index, object] : instance->mObjects)
        {
            if(object)
                { object->Input(inEvent); }
        }
    }
}

