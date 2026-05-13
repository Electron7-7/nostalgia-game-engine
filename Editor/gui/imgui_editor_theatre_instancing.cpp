#include "./imgui_editor.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include "thirdparty/ImGuiFileDialog/ImGuiFileDialog.h"
#include "theatre/editor_theatre.hpp"
#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/theatre/theatre.hpp>
#include <Nostalgia/managers/theatre_manager.hpp>
#include <Nostalgia/settings/engine.hpp>
#include <Nostalgia/application/application.hpp>

namespace IG = ImGui;

void ImGui_Editor::SaveCurrentTheatre()
{
    if(mTheatreState != WANT_SAVE_THEATRE)
    {
        mPreviousTheatreState = mTheatreState;
        mTheatreState = WANT_SAVE_THEATRE;
        IGFD::FileDialogConfig _config{};
        _config.path = ".";
        ImGuiFileDialog::Instance()->OpenDialog("SetFilepathKey",
            "Choose Theatre File",
            ".nt,.theatre,.nostalgiatheatre",
            _config);
    }
}

void ImGui_Editor::CreateNewTheatre()
{
    if(mTheatreState != WANT_NEW_THEATRE)
    {
        mPreviousTheatreState = mTheatreState;
        mTheatreState = WANT_NEW_THEATRE;
    }
}

void ImGui_Editor::LoadNewTheatre()
{
    if(mTheatreState != WANT_LOAD_THEATRE)
    {
        mPreviousTheatreState = mTheatreState;
        mTheatreState = WANT_LOAD_THEATRE;
        IGFD::FileDialogConfig _config{};
        _config.path = ".";
        ImGuiFileDialog::Instance()->OpenDialog("SetFilepathKey",
            "Choose Theatre File",
            ".nt,.theatre,.nostalgiatheatre",
            _config);
    }
}

void ImGui_Editor::PlayCurrentTheatre()
{
    if(mTheatreState != WANT_PLAY_THEATRE)
    {
        mPreviousTheatreState = mTheatreState;
        mTheatreState = WANT_PLAY_THEATRE;
    }
}

void ImGui_Editor::ExitBackToEditor()
{
    if(mTheatreState != WANT_EDIT_THEATRE)
    {
        mPreviousTheatreState = mTheatreState;
        mTheatreState = WANT_EDIT_THEATRE;
    }
}

ImGui_Editor::FileDialogReturnVal ImGui_Editor::_assert_filepath(std::string& ioPath)
{
    FileDialogReturnVal _output{UNDECIDED};
    if(ImGuiFileDialog::Instance()->Display("SetFilepathKey"))
    {
        _output = CANCELLED;
        if(ImGuiFileDialog::Instance()->IsOk())
        {
            ioPath = ImGuiFileDialog::Instance()->GetFilePathName();
            _output = SELECTED;
        }
        ImGuiFileDialog::Instance()->Close();
    }
    return _output;
}

void ImGui_Editor::do_TheatreRelatedPopups()
{
    if(mTheatreState == NO_WANT_AM_SATISFIED)
    {
        if(mPreviousTheatreState == NO_WANT_AM_SATISFIED)
            { return; }
        mTheatreState = mPreviousTheatreState;
    }
    if(mAskAreYouSure)
    {
        if(not IG::IsPopupOpen("_AreYouSure?"))
            { IG::OpenPopup("_AreYouSure?"); }
        if(IG::BeginPopup("_AreYouSure?"))
        {
            IG::SeparatorText("Save changes made to the current Theatre?");
            if(IG::Button("No"))
            {
                mPreviousTheatreState = mTheatreState;
                mCurrentTheatreFilePath.clear();
                mAskAreYouSure = false;
                mIsEditorSaved = true;
                IG::CloseCurrentPopup();
                IG::EndPopup();
                return;
            }
            IG::SameLine();
            if(IG::Button("Cancel"))
            {
                mPreviousTheatreState = mTheatreState = NO_WANT_AM_SATISFIED;
                mAskAreYouSure = true;
                mIsEditorSaved = false;
                IG::CloseCurrentPopup();
                IG::EndPopup();
                return;
            }
            IG::SameLine();
            if(IG::Button("Yes"))
            {
                SaveCurrentTheatre();
                mAskAreYouSure = false;
                IG::CloseCurrentPopup();
                IG::EndPopup();
                return;
            }
            IG::EndPopup();
            return;
        }
    }
    switch(mTheatreState)
    {
    default:
    case NO_WANT_AM_SATISFIED:
        return;
    case WANT_NEW_THEATRE:
        if(not mIsEditorSaved)
        {
            mAskAreYouSure = true;
            return;
        }
        else
        {
            g_pTheatreManager->SetNextTheatreType<EditorTheatre>();
            g_pTheatreManager->LoadFromData(mEditorTheatreData = {});
            mCurrentTheatreFilePath.clear();
            mAskAreYouSure = true;
            mPreviousTheatreState = mTheatreState = NO_WANT_AM_SATISFIED;
        }
        break;
    case WANT_LOAD_THEATRE:
        if(not mIsEditorSaved)
        {
            mAskAreYouSure = true;
            return;
        }
        switch(_assert_filepath(mCurrentTheatreFilePath))
        {
        case SELECTED:
            g_pTheatreManager->LoadFromFile(mCurrentTheatreFilePath);
            mAskAreYouSure = false;
            [[fallthrough]];
        case CANCELLED:
            mTheatreState = mPreviousTheatreState = NO_WANT_AM_SATISFIED;
            [[fallthrough]];
        default:
            break;
        }
        break;
    case WANT_SAVE_THEATRE:
        switch(_assert_filepath(mCurrentTheatreFilePath))
        {
        case SELECTED:
            Theatre::Current()->SaveToFile(mCurrentTheatreFilePath, FileSystem::OverwriteAction::OVERWRITE);
            mTheatreState = mPreviousTheatreState;
            mIsEditorSaved = true;
            [[fallthrough]];
        case CANCELLED:
            mTheatreState = mPreviousTheatreState = NO_WANT_AM_SATISFIED;
            [[fallthrough]];
        default:
            break;
        }
        break;
    case WANT_EDIT_THEATRE:
        if(not Settings::Engine::IsEditorHint)
        {
            g_pTheatreManager->SetNextTheatreType<EditorTheatre>();
            g_pTheatreManager->LoadFromData(mEditorTheatreData = Theatre::Current()->InitialState());
            Settings::Engine::IsEditorHint = true;
            mPreviousTheatreState = mTheatreState = NO_WANT_AM_SATISFIED;
            MainWindow()->SetMouseMode(IWindow::MOUSE_MODE_VISIBLE);
        }
        break;
    case WANT_PLAY_THEATRE:
        if(Settings::Engine::IsEditorHint)
        {
            g_pTheatreManager->SetNextTheatreType<Theatre>();
            g_pTheatreManager->LoadFromData(mEditorTheatreData = Theatre::Current()->CurrentState());
            Settings::Engine::IsEditorHint = false;
            mPreviousTheatreState = mTheatreState = NO_WANT_AM_SATISFIED;
        }
        break;
    }
}
