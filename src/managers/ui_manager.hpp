#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "manager.hpp"
#include "ui/imgui_object.hpp"

class UIManager : public Manager
{
public:
    bool Init();
    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    void Update();
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);
    void Shutdown();

    ImGui_Object* AddImGuiObject(ImGui_Object* NewImGuiObject);
    bool RemoveImGuiObject(ImGui_Object* OldImGuiObject);
    void DrawUI();
    void DrawImGuiUI();

private:
    std::vector<ImGui_Object*> imgui_objects;

    bool is_initialized = false;
};

extern UIManager* global_UIManager;

#endif // UI_MANAGER_H
