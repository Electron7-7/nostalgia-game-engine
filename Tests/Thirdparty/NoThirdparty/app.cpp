#include "../../test_app.hpp"
#include <Nostalgia/application/window.hpp>
#include <Nostalgia/events/event_queue.hpp>
#include <Nostalgia/events/action.hpp>
#include <Nostalgia/managers/physics_manager.hpp>
#include <Nostalgia/managers/theatre_manager.hpp>
#include <Nostalgia/managers/render_manager.hpp>
#include <Nostalgia/managers/input_manager.hpp>
#include <Nostalgia/managers/event_manager.hpp>
#include <Nostalgia/managers/ui_manager.hpp>
#include <Nostalgia/settings/engine.hpp>
#include <Nostalgia/ui/implementor.hpp>
#include <Nostalgia/theatre/thing_factory.hpp>
#include <thread>

const char* TestApplication::Name() { return "TestApplication (NoThirdparty)"; }
void TestApplication::Input(InputEvent*) {}
void TestApplication::Event(AppEvent*) {}

void TestApplication::Stop()
{ IManager::Stop(); }

int TestApplication::Main()
{
    mMainWindow = IWindow::CreateNewWindow(IWindow::Properties{"Thirdparty Test - No Thirdparty"});

    IManager::Add(g_pPhysicsManager);
    IManager::Add(g_pTheatreManager);
    IManager::Add(g_pRenderManager);
    IManager::Add(g_pInputManager);
    IManager::Add(g_pEventManager);
    IManager::Add(g_pUIManager);

    IManager::InitAllManagers();

    std::thread managers_thread{IManager::Start}; // gameloop

    int status{1};

    if(g_pTheatreManager->LoadNewTheatre("theatres/Thirdpartytest_None.nt"))
    {
        std::println("[STATUS] Successful");
        g_pTheatreManager->ShutdownTheatre();
        status = 0;
    }

    Stop();
    managers_thread.join();
    IManager::ShutdownAllManagers();
    IManager::RemoveAll();

    return status;
}
