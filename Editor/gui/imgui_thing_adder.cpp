#include "./imgui_editor.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include "thirdparty/DearImGui/imgui_internal.h"
#include <Nostalgia/theatre/theatre.hpp>
#include <Nostalgia/things/thing_factory.hpp>

using namespace ImGui;

static bool s_TypeButtons(FPID inType, Farg<Tree<PID>> inTypes, PID& outType)
{
    FAUTO _children{inTypes.get_node(inType).children};
    for(FPID _child : _children)
    {
        BeginDisabled(ThingFactory::IsVirtual(_child));
            if(Button(_child.c_name()))
            {
                EndDisabled();
                outType = _child;
                return true;
            }
        EndDisabled();
        Indent();
        if(s_TypeButtons(_child, inTypes, outType))
            { return true; }
        Unindent();
    }
    return false;
}

void ImGui_Editor::do_ThingAdder()
{
    if(mAddThing)
    {
        mAddThing = false;
        OpenPopup("Add Thing");
        SetNextWindowSize({500, 0});
    }
    if(BeginPopupEx(GetCurrentWindow()->GetID("Add Thing"),
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings))
    {
        auto _lock{ThingFactory::LockSortedTypeIDs()};
        FAUTO _types{ThingFactory::GetAllTypes()};
        PID _type{ThingType::Thing};
        if(TreeNode("Resources"))
        {
            if(s_TypeButtons(ThingType::Resource, _types, _type))
            {
                mIsEditorSaved = false;
                Theatre::Current()->CreateThing({_type, _type.name()});
                CloseCurrentPopup();
            }
            TreePop();
        }
        if(TreeNodeEx("Thinkers", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if(s_TypeButtons(ThingType::Thinker, _types, _type))
            {
                mIsEditorSaved = false;
                Theatre::Current()->CreateThing({_type, _type.name()});
                CloseCurrentPopup();
            }
            TreePop();
        }
        EndPopup();
    }
}
