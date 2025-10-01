#ifndef STRING_TO_NUM_H
#define STRING_TO_NUM_H

#include <string>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

#define SHITTY_FUCKING_MACRO(STD, VARIABLE)   \
T new_value = 0;                              \
try                                           \
{                                             \
    new_value = STD(string);                  \
}                                             \
catch(std::invalid_argument const& exception) \
{                                             \
    return false;                             \
}                                             \
VARIABLE = new_value;                         \
return true;

template<typename T>
bool StringToNum(T& number, const std::string& string)
{
    if constexpr(!std::is_integral_v<T>)
        { SHITTY_FUCKING_MACRO(std::stold, number) }
    SHITTY_FUCKING_MACRO(std::stoll, number)
}

template<typename T>
concept GLMContainer = requires
{
    (
        std::is_same_v<std::decay_t<glm::vec2>, std::decay_t<T>> ||
        std::is_same_v<std::decay_t<glm::vec3>, std::decay_t<T>> ||
        std::is_same_v<std::decay_t<glm::vec4>, std::decay_t<T>> ||
        std::is_same_v<std::decay_t<glm::quat>, std::decay_t<T>>
    ) == true;
};

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
