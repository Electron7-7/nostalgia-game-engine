#include "./imgui_editor.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include "thirdparty/DearImGui/imgui_stdlib.h"
#include <Nostalgia/theatre/theatre.hpp>

using namespace ImGui;
using namespace TheatreFile;

void ImGui_Editor::InspectResource()
{
    static ThingData _data{};
    if(mInspectingResourceName.empty())
        { mInspectingResourceName = Theatre::Current()->GetName(mInspectingResourceUID); }
    if(m_sInspectingNewResource)
    {
        m_sInspectingNewResource = false;
        _data.clear();
    }

    if(not Begin(mInspectingResourceName.data(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        { End(); return; }
    else if(Button("Exit"))
    {
        mInspectingResourceUID = ID::Invalid;
        mInspectingResourceName.clear();
        _data.clear();
        End();
        return;
    }
    else if(InspectThing(mInspectingResourceUID,
        Theatre::Current()->GetThing(mInspectingResourceUID)->GetVariables(),
        _data))
        { Theatre::Current()->GetThing(mInspectingResourceUID)->SetVariables(_data); }
    End();
}

void ImGui_Editor::InspectThinker()
{
    static ThingData _data{};
    if(mInspectingThinkerName.empty())
        { mInspectingThinkerName = Theatre::Current()->GetName(mInspectingThinkerUID); }
    if(m_sInspectingNewThinker)
    {
        m_sInspectingNewThinker = false;
        _data.clear();
    }

    if(not Begin(mInspectingThinkerName.data(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        { End(); return; }
    else if(Button("Exit"))
    {
        mInspectingThinkerUID = ID::Invalid;
        mInspectingThinkerName.clear();
        _data.clear();
        End();
        return;
    }
    else if(InspectThing(mInspectingThinkerUID,
        Theatre::Current()->GetThing(mInspectingThinkerUID)->GetVariables(),
        _data))
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
                    std::string _output{_value};
                    outData.set_variable(_output, _name, true);
                    _changed = true;
                }
                break;
            }
        case ThingVarType::Number:
            {
                switch(var.number_type)
                {
                case ThingVariable::NIL:
                default:
                    break;
                case ThingVariable::INT:
                    {
                        int _value{};
                        inData->get_variable(_value, _name);
                        if(DragInt(_name.data(), &_value))
                        {
                            outData.set_variable<int>(_value, _name);
                            _changed = true;
                            break;
                        }
                        break;
                    }
                case ThingVariable::FLOAT:
                    {
                        float _value{};
                        inData->get_variable(_value, _name);
                        if(DragFloat(_name.data(), &_value))
                        {
                            outData.set_variable<float>(_value, _name);
                            _changed = true;
                            break;
                        }
                        break;
                    }
                case ThingVariable::VEC2:
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
                case ThingVariable::VEC3:
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
                case ThingVariable::VEC4:
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
        case ThingVarType::BitMask:
            {
                BitMask _value{};
                inData->get_variable(_value, _name);
                BitMask::StatusArray _status_arr{_value.status()};
                Text("%s", _name.data());
                if(Button("Disable All"))
                {
                    _value.set(BitMask::all_disabled);
                    outData.set_variable(_value, _name);
                    _changed = true;
                }
                SameLine();
                if(Button("Enable All"))
                {
                    _value.set(BitMask::all_enabled);
                    outData.set_variable(_value, _name);
                    _changed = true;
                }
                for(uint i{0}; i < BitMask::max; ++i)
                {
                    if(i > 0 and i != BitMask::max / 2)
                        { SameLine(); }
                    if(Checkbox(std::format("{:2}", i+1).data(), &_status_arr[i]))
                    {
                        _value.set(_status_arr);
                        outData.set_variable(_value, _name);
                        _changed = true;
                    }
                }
                break;
            }
        case ThingVarType::ID:
            {
                ID _uid{};
                inData->get_variable(_uid, var.name);
                std::string _name{Theatre::Current()->GetName(_uid)};
                SelectThing(std::format("{}: {}", var.name, (_name.empty()) ? GlobalConstants::str_NA : _name).data(),
                    _uid,
                    _changed);
                outData.set_variable(_uid, var.name);
                break;
            }
        case ThingVarType::Enum:
            {
                long _value{};
                if(auto _enums{EnumRegistry::GetEnumsOfSameType(var.value)}; not _enums.empty())
                {
                    EnumRegistry::GetEnum(var.value, _value);

                    if(BeginCombo(var.name.data(), var.value.data()))
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
