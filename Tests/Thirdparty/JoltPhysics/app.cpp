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

#ifndef TESTAPP_THEATRE
#   define TESTAPP_THEATRE "../../theatres/ThirdpartyTest_JoltPhysics.nt"
#endif // TESTAPP_THEATRE

const char* TestApplication::Name() { return "TestApplication (JoltPhysics)"; }
void TestApplication::Input(InputEvent*) {}
void TestApplication::Event(AppEvent*) {}

void TestApplication::Stop()
{ IManager::Stop(); }

int TestApplication::Main()
{
    mMainWindow = IWindow::CreateNewWindow(IWindow::Properties{"Thirdparty Test - JoltPhysics Only"});

    IManager::Add(g_pPhysicsManager);
    IManager::Add(g_pTheatreManager);
    IManager::Add(g_pRenderManager);
    IManager::Add(g_pInputManager);
    IManager::Add(g_pEventManager);
    IManager::Add(g_pUIManager);

    IManager::InitAllManagers();

    std::thread managers_thread{IManager::Start}; // gameloop

    int status{1};

    if(g_pTheatreManager->LoadNewTheatre(TESTAPP_THEATRE))
    {
        std::println("[STATUS] Theatre Load Successful");
        auto start{Runtime::Current()};
        while(Runtime::Current() - start < 5.0) {}
        std::println("[STATUS] Timer Wait Successful");
        g_pTheatreManager->ShutdownTheatre();
        std::println("[STATUS] Theatre Shutdown Successful");
        status = 0;
    }
    else
        { print_error("Unable to load testing theatre: {}", TESTAPP_THEATRE); }

    Stop();
    managers_thread.join();
    IManager::ShutdownAllManagers();
    IManager::RemoveAll();

    return status;
}
