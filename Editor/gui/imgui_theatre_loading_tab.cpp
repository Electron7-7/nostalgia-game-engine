#include "./imgui_editor.hpp"
#include "theatre/editor_theatre.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include "thirdparty/DearImGui/imgui_stdlib.h"
#include <Nostalgia/managers/theatre_manager.hpp>
#include <Nostalgia/settings/engine.hpp>

using namespace ImGui;

static const std::map<Theatre::FileOverwriteAction, const char*>
action_type{
    { Theatre::OVERWRITE, "Overwrite"},
    { Theatre::RENAME,    "Rename"},
    { Theatre::CANCEL,    "Cancel"},
};

void ImGui_Editor::TheatreLoadingWindow()
{
    BeginChild("Theatre Debugging");
        InputText("Theatre Load", &m_sTheatreFilePath, ImGuiInputTextFlags_CharsNoBlank);
        InputText("Theatre Save", &m_sTheatreFileSavePath, ImGuiInputTextFlags_CharsNoBlank);

        if(Button("Edit Theatre"))
        {
            m_sLastAttemptedTheatreFilePath = m_sTheatreFilePath;
            LoadEditorTheatre(false);
        }

        if(Button("Run Theatre"))
        {
            m_sLastAttemptedTheatreFilePath = m_sTheatreFilePath;
            LoadTheatre(true);
        }

        BeginDisabled(not Settings::Engine::IsEditorHint);
            if(Button("Run Editor Theatre"))
            {
                mEditorTheatreData = m_spEditorTheatre->CurrentState();
                LoadTheatre(false);
            }
        EndDisabled();
        BeginDisabled(not m_sTheatreRunning);
            SeparatorText("Theatre Saving");

            if(BeginCombo("Duplicate Filename Action", action_type.at(m_sCurrentFileOverwriteAction), ImGuiComboFlags_WidthFitPreview))
            {
                for(FAUTO [action, name] : action_type)
                {
                    const bool _selected{action == m_sCurrentFileOverwriteAction};
                    if(Selectable(name))
                        { m_sCurrentFileOverwriteAction = action; }
                    if(_selected)
                        { SetItemDefaultFocus(); }
                }
                EndCombo();
            }

            if(Button("Save Editor Theatre"))
            {
                m_sLastAttemptedTheatreFilePath = m_sTheatreFileSavePath;
                if(Settings::Engine::IsEditorHint)
                {
                    mEditorTheatreData = m_spEditorTheatre->CurrentState();
                    m_spEditorTheatre->SaveToFile(m_sTheatreFileSavePath, m_sCurrentFileOverwriteAction);
                }
                else
                    { Theatre::Current()->SaveToFile(m_sTheatreFilePath, m_sCurrentFileOverwriteAction); }
            }
        EndDisabled();
        BeginDisabled(not m_sTheatreRunning);
            if(Button("Exit Theatre"))
            {
                Manager::ShutdownTheatre();
                Settings::Engine::IsEditorHint = true;
            }
        EndDisabled();
        BeginDisabled(Settings::Engine::IsEditorHint or not m_sTheatreRunning);
            if(Button("Exit to Editor"))
                { LoadEditorTheatre(true); }
        EndDisabled();
    EndChild();
}
