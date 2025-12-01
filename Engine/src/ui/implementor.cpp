#include "implementor.hpp"
#include "solution.hpp" // IWYU pragma: keep

std::vector<Unique<IUIImplementor>> IUIImplementor::m_sInstances{};

// Constructors/Destructors
IUIImplementor::IUIImplementor(FARG(std::string) inName):
    mID{inName}
{ PRINT_PRETTY_FUNCTION; }

IUIImplementor::~IUIImplementor()
{ PRINT_PRETTY_FUNCTION; }

UI_Implementor::UI_Implementor(FARG(std::string) inName):
    IUIImplementor{inName}
{ PRINT_PRETTY_FUNCTION; }

// Non-static functions
const ID& IUIImplementor::GetID() const
{ return mID; }

IUIImplementor::InstanceState IUIImplementor::GetState() const
{ return mState; }

std::vector<Unique<IUISolution>>& IUIImplementor::GetObjects()
{ return mObjects; }

void UI_Implementor::StopHandlingEvents(bool inStopHandlingEvents)
{ mGlobalCanHandleEvents = !inStopHandlingEvents; }

// Static functions
const std::vector<Unique<IUIImplementor>>& IUIImplementor::GetInstances()
{ return m_sInstances; }

void IUIImplementor::AttachAll()
{ InvokeMethod(&IUIImplementor::Attach); }

void IUIImplementor::DetachAll()
{ InvokeMethod(&IUIImplementor::Detach); }

void IUIImplementor::BeginAll()
{ InvokeMethod(&IUIImplementor::Begin); }

void IUIImplementor::EndAll()
{
    InvokeMethod(&IUIImplementor::End);
    for(auto& instance : m_sInstances)
        { instance->DeactivateOpportunity(); }
}

void IUIImplementor::InvokeMethod(ImplementorFunctionPtr inFunctionPtr)
{
    for(auto& instance : m_sInstances)
        { (instance.get()->*inFunctionPtr)(); }
}

void IUIImplementor::InvokeMethod(SolutionFunction_ptr inFunctionPtr)
{
    for(std::unique_ptr<IUIImplementor>& instance : m_sInstances)
    {
        if(!instance->mAttached)
            { continue; }
        for(auto& object : instance->mObjects)
            { (object.get()->*inFunctionPtr)(); }
    }
}

void IUIImplementor::InvokeMethod(SolutionInputEventFunction_ptr inFunctionPtr, FARG(InputEvent) inEvent)
{
    for(std::unique_ptr<IUIImplementor>& instance : m_sInstances)
    {
        if(!instance->mAttached)
            { continue; }
        for(auto& object : instance->mObjects)
            { (object.get()->*inFunctionPtr)(inEvent); }
    }
}
