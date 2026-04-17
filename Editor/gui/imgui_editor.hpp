#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/ui/solution.hpp>
#include <Nostalgia/theatre/theatre.hpp>
#include <Nostalgia/things/resources/image_texture.hpp>

// Forward Declaration
class EditorTheatre;

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
    static std::string m_sTheatreFilePath,
        m_sTheatreFileSavePath,
        m_sLastAttemptedTheatreFilePath;
    static Theatre::FileOverwriteAction m_sCurrentFileOverwriteAction;
    static EditorTheatre* m_spEditorTheatre;
    static bool m_sTheatreRunning, m_sInspectingNewThinker, m_sInspectingNewResource, m_sAddThing;
    static std::unordered_map<PID, Shared<ImageTexture>> m_sEditorIcons;
    static std::unordered_set<ID> m_sEditorIconUIDs;

    ID mInspectingResourceUID{}, mInspectingThinkerUID{};
    std::string mInspectingThinkerName{}, mInspectingResourceName{};
    TheatreFile::TheatreData mEditorTheatreData{};

    void Viewport3DWindow();
    void Viewport2DWindow();
    void TheatreLoadingWindow();
    void LoadTheatre(bool inLoadFile);
    void LoadEditorTheatre(bool inContinue);
    void ThingAdder();
    void TheatreViewport();
    void TheatreInspector();
    void InspectThinker();
    void InspectResource();
    void SelectThing(const char* inLabel, ID& ioUID, bool& outChanged);
    bool InspectThing(ID inUID, Farg<Shared<TheatreFile::ThingData>> inData, TheatreFile::ThingData& outData);
    void _select_thing(ID, ID&);
    void _thinker_tree_branch(ID);
};

extern ImGui_Editor* g_pImGuiEditor;

#endif // EDITOR_WINDOW_H
