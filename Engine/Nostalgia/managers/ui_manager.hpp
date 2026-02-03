#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <Nostalgia/managers/manager.hpp>

class UIManager : public Manager
{
public:
    consteval const char* DebugName() final { return "UIManager"; }
    bool Init() final;
    ManagerEnums::TheatreReturnValue_t TheatreInit(bool isFirstCall) final;
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool isFirstCall) final;
    void Shutdown() final;
    void Input(InputEvent*) final;

    void DrawUI();
};

extern UIManager* g_pUIManager;

#endif // UI_MANAGER_H
