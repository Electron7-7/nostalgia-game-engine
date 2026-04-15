#include "./imgui_editor.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include "thirdparty/DearImGui/imgui_stdlib.h"
#include <Nostalgia/theatre/theatre.hpp>

using namespace ImGui;
using namespace TheatreFile;

static ID _last_resource_uid{}, _last_thinker_uid{};

void ImGui_Editor::InspectResource()
{
    static ThingData _data{};
    if(_last_resource_uid != mInspectingResourceUID)
    {
        _last_resource_uid = mInspectingResourceUID;
        _data.clear();
    }

    if(not Begin(mInspectingResourceName.data()))
        { End(); return; }
    else if(Button("Exit"))
    {
        mInspectingResourceUID = ID::Invalid;
        mInspectingResourceName.clear();
        End();
        return;
    }
    else if(InspectThing(mInspectingResourceUID, Theatre::Current()->GetThing(mInspectingResourceUID)->GetVariables(), _data))
        { Theatre::Current()->GetThing(mInspectingResourceUID)->SetVariables(_data); }
    End();
}

void ImGui_Editor::InspectThinker()
{
    static ThingData _data{};
    if(_last_thinker_uid != mInspectingThinkerUID)
    {
        _last_thinker_uid = mInspectingThinkerUID;
        _data.clear();
    }

    if(not Begin(mInspectingThinkerName.data()))
        { End(); return; }
    else if(Button("Exit"))
    {
        mInspectingThinkerUID = ID::Invalid;
        mInspectingThinkerName.clear();
        End();
        return;
    }
    else if(InspectThing(mInspectingThinkerUID, Theatre::Current()->GetThing(mInspectingThinkerUID)->GetVariables(), _data))
        { Theatre::Current()->GetThing(mInspectingThinkerUID)->SetVariables(_data); }
    End();
}

bool ImGui_Editor::InspectThing(ID inUID, Farg<Shared<ThingData>> inData, ThingData& outData)
{
    bool _changed{false};
    for(FAUTO var : inData->variables)
    {
        std::string _name{var.name};
        switch(var.type)
        {
        case ThingVarType::Bool:
            {
                bool _value{};
                inData->get_variable(_value, _name);
                if(Checkbox(_name.data(), &_value))
                {
                    outData.set_variable(_value, _name);
                    _changed = true;
                    break;
                }
                break;
            }
        case ThingVarType::String:
            {
                std::string _value{};
                inData->get_variable(_value, _name);
                if(InputText(_name.data(), &_value))
                {
                    outData.set_variable(_value, _name);
                    _changed = true;
                    break;
                }
                break;
            }
        case ThingVarType::Number:
            {
                switch(GetNumberType(var.value))
                {
                case NumberType::NIL:
                default:
                    break;
                case NumberType::INT:
                    {
                        int _value{};
                        inData->get_variable(_value, _name);
                        if(DragInt(_name.data(), &_value))
                        {
                            outData.set_variable(_value, _name);
                            _changed = true;
                            break;
                        }
                        break;
                    }
                case NumberType::FLOAT:
                    {
                        float _value{};
                        inData->get_variable(_value, _name);
                        if(DragFloat(_name.data(), &_value))
                        {
                            outData.set_variable(_value, _name);
                            _changed = true;
                            break;
                        }
                        break;
                    }
                case NumberType::VEC2:
                    {
                        glm::vec2 _value{};
                        inData->get_variable(_value, _name);
                        if(DragGLMv2(_name.data(), &_value))
                        {
                            outData.set_variable(_value, _name);
                            _changed = true;
                            break;
                        }
                        break;
                    }
                case NumberType::VEC3:
                    {
                        glm::vec3 _value{};
                        inData->get_variable(_value, _name);
                        if(DragGLMv3(_name.data(), &_value))
                        {
                            outData.set_variable(_value, _name);
                            _changed = true;
                            break;
                        }
                        break;
                    }
                case NumberType::VEC4:
                    {
                        glm::vec4 _value{};
                        inData->get_variable(_value, _name);
                        if(DragGLMv4(_name.data(), &_value))
                        {
                            outData.set_variable(_value, _name);
                            _changed = true;
                            break;
                        }
                        break;
                    }
                }
                break;
            }
        case ThingVarType::ID:
            {
                ID _value{};
                uint _value_uint{_value()};
                inData->get_variable(_value, var.name);
                PushID(_value());
                DragUInt(var.name.data(), &_value_uint, 0, 0, ImGuiInputTextFlags_ReadOnly);
                PopID();
                SameLine();
                SelectUID("Select Thing", _value, _changed);
                outData.set_variable(_value, var.name);
                break;
            }
        case ThingVarType::Enum:
            {
                long _value{};
                if(auto _enums{EnumRegistry::GetEnumsOfSameType(var.name)}; not _enums.empty())
                {
                    EnumRegistry::GetEnum(var.name, _value);

                    if(BeginCombo("Enum", var.name.data()))
                    {
                        for(FAUTO _enum : _enums)
                        {
                            const bool is_selected{_enum.value == _value};
                            if(Selectable(_enum.name.data()))
                            {
                                outData.set_enum_variable(_enum.name, var.name);
                                _changed = true;
                                break;
                            }
                            if(is_selected)
                                { SetItemDefaultFocus(); }
                        }
                        EndCombo();
                    }
                }
                break;
            }
        case ThingVarType::Child:
        case ThingVarType::Parent:
        case ThingVarType::None:
            break;
        }
    }
    return _changed;
}
