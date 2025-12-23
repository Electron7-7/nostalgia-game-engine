#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include "ui/solution.hpp"
#include "rendering/frame_buffer.hpp"
#include "rendering/texture_buffer.hpp"

class ImGui_Editor : public UI_Solution
{
public:
    Error Init()             final;
    void  Shutdown()         final;
    void  Update()           final;
    void  Input(InputEvent*) final;
    void  OnTheatreEntered() final;
    void  OnTheatreExited()  final;

private:
    Shared<TextureBuffer> mTextureBuffer{nullptr};
};

extern bool gShowDebugWindow;

extern ImGui_Editor* g_pImGuiEditor;

#endif // EDITOR_WINDOW_H
