#ifndef LIGHT_H
#define LIGHT_H

#include "actor.hpp"

enum class LightType
{
    POINT,
    SPOT,
    DIRECTIONAL
};

class light_t : public Actor
{
public:
    static void ClearCounts();

    virtual bool IncrementIndex() = 0;
    virtual LightType Type() const = 0;

    virtual void SetVariables(const ThingData&);
    virtual ThingData GetVariables() const;

    int Index() const;

    glm::vec3 mColor        = glm::vec3(1.0f);
    float mEnergy           = 0.8f;
    float mSpecularStrength = 1.0f;
    float mAmbientStrength  = 0.05f;
    float mAttenuation      = 1.0f;
    float mRange            = 100.0f;
    float mSpotAngle        = 17.5f;
    float mSpotAngleFade    = 5.0f;
    bool  mEnabled          = true;

protected:
    int mIndex = 0;

    static int sPointCount;
    static int sSpotCount;
    static int sDirectionalCount;

    static constexpr int MAX_POINT = 48;
    static constexpr int MAX_SPOT  = 48;
    static constexpr int MAX_DIRECTIONAL = 4;
};

class PointLight : public light_t
{
public:
    static int GetCount();
    LightType Type() const final;
    bool IncrementIndex() final;
};

class SpotLight : public light_t
{
public:
    static int GetCount();
    void SetupVariables(const ThingData&);
    LightType Type() const final;
    bool IncrementIndex() final;
};

class DirectionalLight : public light_t
{
public:
    static int GetCount();
    void SetupVariables(const ThingData&);
    LightType Type() const final;
    bool IncrementIndex() final;
};

#endif // LIGHT_H
