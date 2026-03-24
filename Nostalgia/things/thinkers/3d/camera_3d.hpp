#ifndef CAMERA_3D_H
#define CAMERA_3D_H

#include <Nostalgia/things/thinkers/3d/actor_3d.hpp>
#include <Nostalgia/rendering/environment.hpp>

class Camera3D : public Actor3D
{
public:
    SUPER(Actor3D)
    READY_OVERRIDE
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    float mFOV{75.0f},
        mViewCutoffNear{0.01f},
        mViewCutoffFar{1000.0f};
    Environment mEnvironment{};

    virtual ID ViewportID() const;

    virtual bool Current() const;
    virtual Error SetCurrent(bool);

    virtual BitMask LayersMask() const;
    virtual void SetLayersMask(BitMask inRenderLayersMask);

    glm::mat4 ViewMatrix() const;
    glm::mat4 ProjectionMatrix() const;

    ID EditorMeshInstanceID() const;

protected:
    bool mInitCurrent{true};
    ID mViewportID{UID::o_RootViewport};
    BitMask mLayersMask{};
    ID mEditorMeshInstanceID{};

    virtual void OnAncestorRemoved(Relative) override;
    virtual void OnAncestorAdded(Relative) override;
};

#endif // CAMERA_3D_H
