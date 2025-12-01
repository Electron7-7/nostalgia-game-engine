#ifndef MONITOR_H
#define MONITOR_H

#include "core/type_helpers.hpp"

#include <string>
#include <memory>

struct Monitor
{
    MAKE_TYPE_ARG(
        VideoMode,
            int width{0};
            int height{0};
            int bit_depth[3]{0};
            float refresh_rate{0.0f};,
        video_mode{})

    MAKE_TYPE_ARG(
        PhysicalSize,
            int width_mm{0}; int height_mm{0};,
        physical_size{})

    MAKE_TYPE_ARG(
        ContentScale,
            float x{0.0f}; float y{0.0f};,
        content_scale{})

    MAKE_TYPE_ARG(
        VirtualPosition,
            int x{0}; int y{0};,
        virtual_position{})

    MAKE_TYPE_ARG(
        WorkArea,
            int xpos{0}; int ypos{0};
            int width{0}; int height{0};,
        work_area{})

    std::string title{"Untitled"};
    void* native_monitor{nullptr};

    Monitor() = default;
    Monitor(void* inNativeMonitor, FARG(std::string) inTitle = "Untitled"):
        title{inTitle}, native_monitor{inNativeMonitor} {}

    // Used by `std::find` in 'Engine/src/backends/glfw/glfw_window.cpp:34'
    constexpr bool operator==(FARG(std::unique_ptr<Monitor>) other) const
    { return native_monitor == other->native_monitor; }
};


#endif // MONITOR_H
