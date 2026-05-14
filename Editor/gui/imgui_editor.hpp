#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/ui/solution.hpp>
#include <Nostalgia/things/resources/image_texture.hpp>
#include <Nostalgia/theatre/theatre_file.hpp>

// Forward Declaration
class EditorTheatre;

class ImGui_Editor : public UI_Solution
{
public:
    void Init()             final;
    void Shutdown()         final;
    void Update()           final;
    void Input(InputEvent*) final;
    void Event(IEvent*)     final;
    void TheatreEntered()   final;
    void TheatreExited()    final;

private:
    enum TheatreState
    {
        WANT_NEW_THEATRE,
        WANT_LOAD_THEATRE,
        WANT_SAVE_THEATRE,
        WANT_PLAY_THEATRE,
        WANT_EDIT_THEATRE,
        NO_WANT_AM_SATISFIED
    };

    enum FileDialogReturnVal
    {
        SELECTED,
        CANCELLED,
        UNDECIDED
    };

    TheatreState mTheatreState{NO_WANT_AM_SATISFIED};
    TheatreState mPreviousTheatreState{NO_WANT_AM_SATISFIED};
    bool mAskAreYouSure{true},
        mIsEditorSaved{false},
        mTheatreRunning{false},
        mInspectingNewThing{false},
        mAddThing{false},
        mThingAdderOpened{false},
        mResizeEditorWindows{true};
    std::string mCurrentTheatreFilePath{}, mScreenshotFilePath{};
    std::unordered_map<PID, Shared<ImageTexture>> mEditorIcons{}, mNewEditorIcons{};
    TheatreFile::TheatreData mEditorTheatreData{};
    ID mInspectingThingUID{};
    Shared<TheatreFile::ThingData> m_pInspectingThingData{MakeShared<TheatreFile::ThingData>()};

    FileDialogReturnVal _assert_filepath(std::string& ioPath);
    void SaveCurrentTheatre();
    void CreateNewTheatre();
    void LoadNewTheatre();
    void PlayCurrentTheatre();
    void ExitBackToEditor();
    void do_TheatreRelatedPopups();
    void do_ThingAdder();

    uint GetIconTextureBufferID(FPID inType);
    void Viewport3DWindow(EditorTheatre*);
    void Viewport2DWindow(EditorTheatre*);
    void TheatreViewport();
    void TheatreTree();
    void TheatreInspector();
    bool SelectThing(const char* inLabel, ID& ioUID);
    void InspectThing();
    bool _select_thing(ID);
    void _thinker_tree_branch(ID);
};

extern ImGui_Editor* g_pImGuiEditor;

#endif // EDITOR_WINDOW_H
