#ifndef RENDERING_DEVICE_H
#define RENDERING_DEVICE_H

#include "../common.hpp"

// Heavily plagiarizing Godot's 'RenderingDevice'
class RenderingDevice: public RenderingCommons
{
private:
    static RenderingDevice* s_pSingleton;

    // RenderingContextDriver*
    // RenderingDeviceDriver*
    // RenderingContextDriver::Device
    // bool mIsMainInstance{false};
    // bool mLocalDeviceProcessing{false};
};

#endif
