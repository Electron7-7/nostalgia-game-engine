#ifndef STRING_TO_NUM_H
#define STRING_TO_NUM_H

#include "glm_to_string.hpp"

#include <stdexcept>

template<typename T>
T StringToNum(const std::string& string)
{
    T output{static_cast<T>(-1)}; // Because this is used a lot on IDs, and '-1' == `ID::Invalid`
    if constexpr(!std::is_integral_v<T>)
    {
        try { output = std::stold(string); }
        catch(std::invalid_argument const& e) {}
    }
    else
    {
        try { output = std::stoll(string); }
        catch(std::invalid_argument const& e) {}
    }
    return output;
}

template<typename T>
bool StringToNum(T& number, const std::string& string)
{
    T try_number{0};
    if constexpr(!std::is_integral_v<T>)
    {
        try { try_number = std::stold(string); }
        catch(std::invalid_argument const& e) { return false; }
    }
    else
    {
        try { try_number = std::stoll(string); }
        catch(std::invalid_argument const& e) { return false; }
    }
    number = try_number;
    return true;
}

template<GLMContainer T, unsigned int size>
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
                if(!StringToNum(temp_num, buffer))
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
    if(!StringToNum(temp_num, buffer))
        { return false; }
    output[size - 1] = temp_num;
    variable = output;
    return true;
}

template<>
inline bool StringToNum(glm::vec2& variable, const std::string& string)
{ return InterpretGLM<glm::vec2, 2>(variable, string); }

template<>
inline bool StringToNum(glm::vec3& variable, const std::string& string)
{ return InterpretGLM<glm::vec3, 3>(variable, string); }

template<>
inline bool StringToNum(glm::vec4& variable, const std::string& string)
{ return InterpretGLM<glm::vec4, 4>(variable, string); }

template<>
inline bool StringToNum(glm::quat& variable, const std::string& string)
{ return InterpretGLM<glm::quat, 4>(variable, string); }

#endif // STRING_TO_NUM_H
