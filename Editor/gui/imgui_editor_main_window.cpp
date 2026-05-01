#include "./imgui_editor.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include "thirdparty/DearImGui/imgui_stdlib.h"
#include <Nostalgia/theatre/theatre.hpp>
#include <Nostalgia/settings/engine.hpp>

using namespace ImGui;
using namespace TheatreFile;

bool ImGui_Editor::InspectThing(ID inUID, Farg<Shared<ThingData>> inData, ThingData& outData)
{
    bool _changed{false};
    for(FAUTO var : inData->variables)
    {
        if(var.editor_ignored)
            { continue; }
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
                    outData.set_variable(_output, _name);
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
                SeparatorText(_name.data());
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
                auto& _style{GetStyle()};
                static float _checkbox_width{GetFrameHeight() + _style.ItemInnerSpacing[0]
                    + CalcTextSize("00")[0]};
                float _window_width{GetWindowSize()[0] - _style.WindowBorderSize - _style.FramePadding[0]};
                float _current_width{_style.FramePadding[0] + _checkbox_width};

                for(uint i{0}; i < BitMask::max; ++i)
                {
                    if(Checkbox(std::format("{:2}", i+1).data(), &_status_arr[i]))
                    {
                        _value.set(_status_arr);
                        outData.set_variable(_value, _name);
                        _changed = true;
                    }
                    _current_width += _style.ItemSpacing[0] + _checkbox_width;
                    if(_current_width >= _window_width)
                        { _current_width = _style.FramePadding[0] + _checkbox_width; }
                    else if(i != BitMask::max - 1)
                        { SameLine(); }
                }
                break;
            }
        case ThingVarType::ID:
            {
                ID _uid{};
                inData->get_variable(_uid, var.name);
                std::string _name{Theatre::Current()->GetName(_uid)};
                SelectThing(std::format("{}: {}",
                    var.name,
                    (_name.empty()) ? GlobalConstants::str_NA : _name).data(),
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
