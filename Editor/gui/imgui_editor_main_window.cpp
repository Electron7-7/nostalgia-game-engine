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
        if(not (var.usage_flags & VARIABLE_USAGE_EDITOR))
            { continue; }
        std::string _name{var.name};
        switch(var.value.type())
        {
        default:
            break;
        case Variant::BOOL:
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
        case Variant::STRING:
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
        case Variant::INT:
            {
                if(var.hint == VARIABLE_HINT_FLAGS)
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
                }
                else if(var.hint == VARIABLE_HINT_THING_UID)
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
                }
                else if(var.hint == VARIABLE_HINT_ENUM)
                {
                    long _value{};
                    if(not var.hint_string.empty())
                    {
                        std::string _buffer{};
                        std::map<long, std::string> _enums{};
                        for(auto iter{var.hint_string.begin()}; iter != var.hint_string.end(); ++iter)
                        {
                            if(*iter == ',' or iter == var.hint_string.end())
                            {
                                long _value{};
                                EnumRegistry::GetEnum(_buffer, _value);
                                _enums[_value] = _buffer;
                                _buffer.clear();
                                continue;
                            }
                            _buffer += *iter;
                        }

                        if(BeginCombo(var.name.data(), _enums[var.value.operator int()].data()))
                        {
                            for(FAUTO [enum_value, enum_name] : _enums)
                            {
                                const bool is_selected{enum_value == _value};
                                if(Selectable(enum_name.data()))
                                {
                                    outData.set_variable(static_cast<int>(enum_value), var.name);
                                    _changed = true;
                                    break;
                                }
                                if(is_selected)
                                    { SetItemDefaultFocus(); }
                            }
                            EndCombo();
                        }
                    }
                }
                else
                {
                    int _value{};
                    inData->get_variable(_value, _name);
                    if(DragInt(_name.data(), &_value))
                    {
                        outData.set_variable(_value, _name);
                        _changed = true;
                        break;
                    }
                }
                break;
            }
        case Variant::FLOAT:
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
        case Variant::VECTOR2:
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
        case Variant::VECTOR3:
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
        case Variant::VECTOR4:
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
        case Variant::THING:
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
            }
        }
    }
    return _changed;
}
