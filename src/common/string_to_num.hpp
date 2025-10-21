#ifndef STRING_TO_NUM_H
#define STRING_TO_NUM_H

#include "math/glm_concepts.hpp"

#include <stdexcept>

template<GLMContainer T>
constexpr ushort gGlmSize()
{
    if constexpr(GLM_Vec2<T>)
        { return 2; }
    else if constexpr(GLM_Vec3<T>)
        { return 3; }
    else if constexpr(GLM_Vec4<T> || GLM_Quat<T>)
        { return 4; }
}

template<typename T>
bool StringToNum(T& output, const std::string& string)
{
    T number{static_cast<T>(-1)}; // Because this is used a lot on IDs, and '-1' == `ID::Invalid`
    if constexpr(!std::is_integral_v<T>)
    {
        try { number = std::stold(string); }
        catch(std::invalid_argument const& e) { return false; }
    }
    else
    {
        try { number = std::stoll(string); }
        catch(std::invalid_argument const& e) { return false; }
    }
    output = number;
    return true;
}

template<GLMContainer T, uint size = gGlmSize<T>()>
bool InterpretGLM(T& variable, const std::string& string)
{
    T output = T();
    std::string buffer = "";
    size_t index = 0;
    for(const char& character : string)
    {
        switch(character)
        {
        case ',':
            if(index < (size - 1))
            {
                float temp_num = output[index];
                if(!StringToNum<float>(temp_num, buffer))
                    { return false; }
                output[index++] = temp_num;
            }
            buffer.clear();
            [[fallthrough]];
        case ' ':
            continue;
        default:
            buffer += character;
            continue;
        }
    }
    float temp_num = output[size - 1];
    if(!StringToNum<float>(temp_num, buffer))
        { return false; }
    output[size - 1] = temp_num;
    variable = output;
    return true;
}

template<>
inline bool StringToNum(glm::vec2& output, const std::string& string)
{ return InterpretGLM(output, string); }

template<>
inline bool StringToNum(glm::vec3& output, const std::string& string)
{ return InterpretGLM(output, string); }

template<>
inline bool StringToNum(glm::vec4& output, const std::string& string)
{ return InterpretGLM(output, string); }

template<>
inline bool StringToNum(glm::quat& output, const std::string& string)
{ return InterpretGLM(output, string); }

#endif // STRING_TO_NUM_H
