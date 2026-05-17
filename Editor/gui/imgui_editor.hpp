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

    void SetStartupTheatre(Sarg inFilePath);
    void QuitEditor();
    bool ReadyToQuit();

private:
    enum TheatreState
    {
        WANT_NEW_THEATRE,
        WANT_LOAD_THEATRE,
        WANT_SAVE_THEATRE,
        WANT_PLAY_THEATRE,
        WANT_EDIT_THEATRE,
        WANT_QUIT_EDITOR,
        NO_WANT_AM_SATISFIED,
    };

    enum FileDialogReturnVal
    {
        SELECTED,
        CANCELLED,
        UNDECIDED
    };

    inline static std::unordered_map<PID, Shared<ImageTexture>> m_sEditorIcons{};

    TheatreState mTheatreState{NO_WANT_AM_SATISFIED};
    TheatreState mPreviousTheatreState{NO_WANT_AM_SATISFIED};
    bool mAskAreYouSure{true},
        mIsEditorSaved{false},
        mTheatreRunning{false},
        mInspectingNewThing{false},
        mAddThing{false},
        mThingAdderOpened{false},
        mResizeEditorWindows{true},
        mViewTheatreInFullbright{false},
        mReadyToQuit{false};

    std::string mCurrentTheatreFilePath{}, mScreenshotFilePath{};
    TheatreFile::TheatreData mEditorTheatreData{};
    ID mInspectingThingUID{};
    Shared<TheatreFile::ThingData> m_pInspectingThingDataIn{MakeShared<TheatreFile::ThingData>()};
    TheatreFile::ThingData mInspectingThingDataOut{};

    FileDialogReturnVal _assert_filepath(std::string& ioPath);
    void SaveCurrentTheatre();
    void CreateNewTheatre();
    void LoadNewTheatre();
    void PlayCurrentTheatre();
    void ExitBackToEditor();
    void do_TheatreRelatedPopups();
    void do_ThingAdder();
    bool set_TheatreState(TheatreState);

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
