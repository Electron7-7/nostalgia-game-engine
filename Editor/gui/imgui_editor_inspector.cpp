#include "./imgui_editor.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include "thirdparty/DearImGui/imgui_stdlib.h"
#include "thirdparty/ImGuiFileDialog/ImGuiFileDialog.h"
#include <Nostalgia/things/thing_factory.hpp>
#include <Nostalgia/settings/engine.hpp>
#include <Nostalgia/theatre/theatre.hpp>

using namespace ImGui;
using namespace TheatreFile;

void ImGui_Editor::InspectThing()
{
    static bool _update_thing{false};
    static ID _parent{};
    if(mInspectingNewThing)
    {
        m_pInspectingThingDataIn = ThingFactory::GetThing(mInspectingThingUID)->GetVariables();
        mInspectingThingDataOut  = {m_pInspectingThingDataIn->type, m_pInspectingThingDataIn->name};
        _parent = m_pInspectingThingDataIn->get_parent();
        mInspectingNewThing = false;
    }

    if(ThingFactory::IsThinker(m_pInspectingThingDataIn->type)
        and SelectThing(std::format("Parent: {}", (_parent.invalid())
                ? "None"
                : ThingFactory::GetName(_parent)).data(),
            _parent))
    {
        Theatre::Current()->SetParent(mInspectingThingUID, _parent);
        _parent = m_pInspectingThingDataIn->get_parent();
        mIsEditorSaved = false;
    }

    for(FAUTO var : m_pInspectingThingDataIn->variables)
    {
        if(not (var.usage_flags & VARIABLE_USAGE_EDITOR))
            { continue; }
        else if(var.hint == TheatreFile::VARIABLE_HINT_THING_REFERENCE)
        {
            ID _uid{};
            std::string _name{};
            switch(var.value.type())
            {
            default:
                continue;
            case Variant::FLOAT:
            case Variant::INT:
                m_pInspectingThingDataIn->get_variable(_uid, var.name);
                _name = ThingFactory::GetName(_uid);
                break;
            case Variant::THING:
            case Variant::STRING:
                m_pInspectingThingDataIn->get_variable(_name, var.name);
                _uid = ThingFactory::GetUID(_name);
                break;
            }
            if(SelectThing(std::format("{}: {}",
                var.name,
                (_name.empty()) ? GlobalConstants::str_NA : _name).data(),
                _uid))
            {
                mInspectingThingDataOut.set_variable(_uid, var.name);
                _update_thing = true;
            }
            continue;
        }
        std::string _name{var.name};
        switch(var.value.type())
        {
        default:
            break;
        case Variant::BOOL:
            {
                bool _value{};
                m_pInspectingThingDataIn->get_variable(_value, _name);
                if(Checkbox(_name.data(), &_value))
                {
                    mInspectingThingDataOut.set_variable(_value, _name);
                    _update_thing = true;
                    break;
                }
                break;
            }
        case Variant::STRING:
            {
                std::string _value{};
                m_pInspectingThingDataIn->get_variable(_value, _name);
                if(InputText(_name.data(), &_value))
                    { mInspectingThingDataOut.set_variable(_value, _name); }
                if(IsItemDeactivatedAfterEdit())
                    { _update_thing = true; }
                if(var.hint == VARIABLE_HINT_FILE)
                {
                    SameLine();
                    if(Button("Open") and not ImGuiFileDialog::Instance()->IsOpened("GetFileKey"))
                    {
                        IGFD::FileDialogConfig _config{};
                        _config.path = ".";
                        _config.flags = ImGuiFileDialogFlags_Modal;
                        ImGuiFileDialog::Instance()->OpenDialog("GetFileKey",
                            "Choose File", var.hint_string.data(), _config);
                    }
                    if(ImGuiFileDialog::Instance()->Display("GetFileKey"))
                    {
                        if(ImGuiFileDialog::Instance()->IsOk())
                        {
                            mInspectingThingDataOut.set_variable(_value = ImGuiFileDialog::Instance()->GetFilePathName(),
                                _name);
                            _update_thing = true;
                        }
                        ImGuiFileDialog::Instance()->Close();
                    }
                }
                break;
            }
        case Variant::INT:
            {
                if(var.hint == VARIABLE_HINT_FLAGS)
                {
                    BitMask _value{};
                    m_pInspectingThingDataIn->get_variable(_value, _name);
                    BitMask::StatusArray _status_arr{_value.status()};
                    SeparatorText(_name.data());
                    if(Button("Disable All"))
                    {
                        _value.set(BitMask::all_disabled);
                        mInspectingThingDataOut.set_variable(_value, _name);
                        _update_thing = true;
                    }
                    SameLine();
                    if(Button("Enable All"))
                    {
                        _value.set(BitMask::all_enabled);
                        mInspectingThingDataOut.set_variable(_value, _name);
                        _update_thing = true;
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
                            mInspectingThingDataOut.set_variable(_value, _name);
                            _update_thing = true;
                        }
                        _current_width += _style.ItemSpacing[0] + _checkbox_width;
                        if(_current_width >= _window_width)
                            { _current_width = _style.FramePadding[0] + _checkbox_width; }
                        else if(i != BitMask::max - 1)
                            { SameLine(); }
                    }
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
#pragma message("TODO: maybe add \\0 to the end of the enum hint string and check for ',' or '\\0'")
                            if(*iter != ',')
                                { _buffer += *iter; }
                            if(*iter == ',' or iter == var.hint_string.end() - 1)
                            {
                                long _value{};
                                EnumRegistry::GetEnum(_buffer, _value);
                                _enums[_value] = _buffer;
                                _buffer.clear();
                                continue;
                            }
                        }

                        if(BeginCombo(var.name.data(),
                            (_enums.contains(var.value.operator int()))
                                ? _enums[var.value.operator int()].data() : ""))
                        {
                            for(FAUTO [enum_value, enum_name] : _enums)
                            {
                                const bool is_selected{enum_value == _value};
                                if(Selectable(enum_name.data()))
                                {
                                    mInspectingThingDataOut.set_variable(static_cast<int>(enum_value), var.name);
                                    _update_thing = true;
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
                    m_pInspectingThingDataIn->get_variable(_value, _name);
                    if(DragInt(_name.data(), &_value))
                    {
                        mInspectingThingDataOut.set_variable(_value, _name);
                        _update_thing = true;
                    }
                }
                break;
            }
        case Variant::FLOAT:
            {
                float _value{};
                m_pInspectingThingDataIn->get_variable(_value, _name);
                if(DragFloat(_name.data(), &_value))
                {
                    mInspectingThingDataOut.set_variable(_value, _name);
                    _update_thing = true;
                }
                break;
            }
        case Variant::VECTOR2:
            {
                glm::vec2 _value{};
                m_pInspectingThingDataIn->get_variable(_value, _name);
                if(DragGLMv2(_name.data(), &_value))
                {
                    mInspectingThingDataOut.set_variable(_value, _name);
                    _update_thing = true;
                }
                break;
            }
        case Variant::VECTOR3:
            {
                glm::vec3 _value{};
                m_pInspectingThingDataIn->get_variable(_value, _name);
                if(DragGLMv3(_name.data(), &_value))
                {
                    mInspectingThingDataOut.set_variable(_value, _name);
                    _update_thing = true;
                }
                break;
            }
        case Variant::VECTOR4:
        case Variant::QUATERNION:
            {
                glm::vec4 _value{};
                m_pInspectingThingDataIn->get_variable(_value, _name);
                if(DragGLMv4(_name.data(), &_value))
                {
                    mInspectingThingDataOut.set_variable(_value, _name);
                    _update_thing = true;
                    break;
                }
                break;
            }
        case Variant::THING:
            {
                ID _uid{};
                m_pInspectingThingDataIn->get_variable(_uid, var.name);
                std::string _name{ThingFactory::GetName(_uid)};
                if(SelectThing(std::format("{}: {}",
                    var.name,
                    (_name.empty()) ? GlobalConstants::str_NA : _name).data(),
                    _uid))
                    { _update_thing = true; }
                mInspectingThingDataOut.set_variable(_uid, var.name);
            }
        }
    }
    if(_update_thing)
    {
        ThingFactory::GetThing(mInspectingThingUID)->SetVariables(mInspectingThingDataOut);
        mInspectingNewThing = not (_update_thing = mIsEditorSaved = false);
    }
}
