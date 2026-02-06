#ifndef MONITOR_H
#define MONITOR_H

#ifdef NOSTALGIA_GCC_PCH
#   include <Nostalgia/Nostalgia.hpp>
#endif

struct Monitor
{
    MAKE_STRUCT_ARG(
        VideoMode,
            int width{0};
            int height{0};
            int bit_depth[3]{0};
            float refresh_rate{0.0f};,
        video_mode{})

    MAKE_STRUCT_ARG(
        PhysicalSize,
            int width_mm{0}; int height_mm{0};,
        physical_size{})

    MAKE_STRUCT_ARG(
        ContentScale,
            float x{0.0f}; float y{0.0f};,
        content_scale{})

    MAKE_STRUCT_ARG(
        VirtualPosition,
            int x{0}; int y{0};,
        virtual_position{})

    MAKE_STRUCT_ARG(
        WorkArea,
            int xpos{0}; int ypos{0};
            int width{0}; int height{0};,
        work_area{})

    std::string title{"Untitled"};
    void* native_monitor{nullptr};

    Monitor() = default;
    Monitor(void* inNativeMonitor, Sarg inTitle = "Untitled"):
        title{inTitle}, native_monitor{inNativeMonitor} {}

    // Used by `std::find` in 'Engine/src/backends/glfw/glfw_window.cpp:34'
    constexpr bool operator==(Farg<Unique<Monitor>> other) const
    { return native_monitor == other->native_monitor; }
};

#endif // MONITOR_H
