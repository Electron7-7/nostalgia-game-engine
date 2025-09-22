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

    virtual void SetupVariables(const data_t&);

    int Index() const;
    bool Enabled() const;

    glm::vec3 m_Color        = glm::vec3(1.0f);
    float m_Energy           = 0.8f;
    float m_SpecularStrength = 1.0f;
    float m_AmbientStrength  = 0.05f;
    float m_Attenuation      = 1.0f;
    float m_Range            = 100.0f;
    float m_SpotAngle        = 17.5f;
    float m_SpotAngleFade    = 5.0f;

protected:
    bool m_Enabled = true;
    int m_Index = 0;

    static int s_PointCount;
    static int s_SpotCount;
    static int s_DirectionalCount;

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
    void SetupVariables(const data_t&);
    LightType Type() const final;
    bool IncrementIndex() final;
};

class DirectionalLight : public light_t
{
public:
    static int GetCount();
    LightType Type() const final;
    bool IncrementIndex() final;
};

#endif // LIGHT_H
