#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/ui/solution.hpp>
#include <Nostalgia/fwd/theatre.hpp>

class ImGui_Editor : public UI_Solution
{
public:
    void Init()             final;
    void Shutdown()         final;
    void Update()           final;
    void Input(InputEvent*) final;
    void TheatreEntered() final;
    void TheatreExited()  final;

private:
    static uint GetIconTextureBufferID(FPID inType);

    ID mInspectingResourceUID{}, mInspectingThinkerUID{};
    std::string mInspectingThinkerName{}, mInspectingResourceName{};

    void ThingAdder();
    void TheatreInspector();
    void InspectResource();
    void InspectThinker();
    void SelectUID(const char* inLabel, ID& ioUID, bool& outChanged);
    bool InspectThing(ID inUID, Farg<Shared<TheatreFile::ThingData>> inData, TheatreFile::ThingData& outData);
    void _display_thinker(ID);
    void _thinker_tree_branch(ID);
    void _resource_list_item(ID);
};

extern bool gShowDebugWindow,
    gDebugConsoleOpened,
    gTheatreInspectorActive;

extern ImGui_Editor* g_pImGuiEditor;

#endif // EDITOR_WINDOW_H
