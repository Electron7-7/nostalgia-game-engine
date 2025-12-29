#ifndef LIGHT_H
#define LIGHT_H

#include "things/actors/actor.hpp"

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

    virtual bool IncrementIndex()  = 0;
    virtual LightType Type() const = 0;

    virtual void Ready() override;
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

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

class PointLight : public light_t
{
public:
    static int GetCount();
    bool IncrementIndex() final;
    LightType Type() const final { return LightType::POINT; }
};

class SpotLight : public light_t
{
public:
    static int GetCount();
    void SetVariables(Farg<ThingData>) override;
    Shared<ThingData> GetVariables() const override;
    bool IncrementIndex() final;
    LightType Type() const final { return LightType::SPOT; }
};

class DirectionalLight : public light_t
{
public:
    static int GetCount();
    void SetVariables(Farg<ThingData>) override;
    Shared<ThingData> GetVariables() const override;
    bool IncrementIndex() final;
    LightType Type() const final { return LightType::DIRECTIONAL; }
};

#endif // LIGHT_H
