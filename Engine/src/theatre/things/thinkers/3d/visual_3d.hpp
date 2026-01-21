#ifndef VISUAL_3D_H
#define VISUAL_3D_H

#include "core/bitmask.hpp"
#include "core/uid.hpp"
#include "theatre/things/thinkers/3d/actor_3d.hpp"

class Visual3D : public Actor3D
{
public:
    bool mOverrideEnableDebugHighlight{false};
    glm::vec4 mDebugHighlight{1.0f, 0.2f, 0.9f, 1.0f};

    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;

    virtual BitMask Layers() const;
    virtual void SetLayers(BitMask inVisualLayers);

    virtual ID Viewport() const;
    virtual void Viewport(ID);

protected:
    BitMask mVisualLayers{};
    ID mViewportID{UID::a_Global3DViewport};

    virtual void OnAncestorRemoved(Relative) override;
    virtual void OnAncestorAdded(Relative) override;
};

#endif // VISUAL_3D_H
