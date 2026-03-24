#ifndef LIGHT_H
#define LIGHT_H

#include <Nostalgia/things/thinkers/3d/visual_3d.hpp>

enum class LightType : int
{
    POINT,
    SPOT,
    DIRECTIONAL,
    NONE,
};

class Light3D : public Visual3D
{
public:
    SUPER(Visual3D)
    READY_OVERRIDE
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    static void ClearCounts();

    virtual bool IncrementIndex() { return false; }
    virtual LightType Type() const { return LightType::NONE; }

    int Index() const;

    glm::vec3 mColor        { 1.0f   };
    float mEnergy           { 0.8f   };
    float mSpecularStrength { 1.0f   };
    float mAmbientStrength  { 0.05f  };
    float mAttenuation      { 1.0f   };
    float mRange            { 100.0f };
    float mSpotAngle        { 17.5f  };
    float mSpotAngleFade    { 5.0f   };
    bool  mEnabled          { true   };

protected:
    int mIndex{0};

    static int sPointCount;
    static int sSpotCount;
    static int sDirectionalCount;

    static constexpr int MAX_POINT {48};
    static constexpr int MAX_SPOT  {48};
    static constexpr int MAX_DIRECTIONAL {4};
};

class PointLight3D : public Light3D
{
public:
    SUPER(Light3D)

    static int GetCount();

    bool IncrementIndex() final;
    LightType Type() const final { return LightType::POINT; }
};

class SpotLight3D : public Light3D
{
public:
    SUPER(Light3D)

    static int GetCount();
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    bool IncrementIndex() final;
    LightType Type() const final { return LightType::SPOT; }
};

class DirectionalLight3D : public Light3D
{
public:
    SUPER(Light3D)

    static int GetCount();
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    bool IncrementIndex() final;
    LightType Type() const final { return LightType::DIRECTIONAL; }
};

#endif // LIGHT_H
