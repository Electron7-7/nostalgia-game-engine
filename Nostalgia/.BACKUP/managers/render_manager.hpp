#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include <Nostalgia/managers/manager.hpp>
#include <Nostalgia/rendering/renderer_api.hpp>

class RenderManager : public Manager
{
public:
    constexpr const char* DebugName() final { return "RenderManager"; }

    bool Init() final;
    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall) final;
    void Update() final;
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall) final;
    void Shutdown() final;

    void CalculateFrameTime(bool);
    bool CalculatingFrameTime() const;
    double GetTheatreFrameTime() const;
    double GetUIFrameTime() const;
    void SetAutomaticWindowClear(bool);

private:
    bool mCanClearWindow{true}, mCanCalculateFrametime{false};
    double mTheatreFrametime{0.0}, mUIFrametime{0.0};
};

// Singleton accessor
extern RenderManager* g_pRenderManager;

#endif // RENDER_MANAGER_H
