#include "window_info.hpp"
#include "thirdparty/DearImGui/imgui.h"

scale_t::scale_t(const ImVec2& imvec2)
:   width(static_cast<int>(imvec2.x)), height(static_cast<int>(imvec2.y)) {}

scale_t::operator ImVec2() const
{ return ImVec2{static_cast<float>(width), static_cast<float>(height)}; }

position_t::position_t(const ImVec2& imvec2)
:   x{static_cast<int>(imvec2.x)}, y{static_cast<int>(imvec2.y)} {}

position_t::operator ImVec2() const
{ return ImVec2{static_cast<float>(x), static_cast<float>(y)}; }
