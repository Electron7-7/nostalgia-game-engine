#ifndef STRING_TO_NUM_H
#define STRING_TO_NUM_H

#include <stdexcept>
#include <string>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

// FIXME: I hate doing this
inline unsigned long std_stoul(const std::string& string)
{ return std::stoul(string); }
inline long std_stol(const std::string& string)
{ return std::stol(string); }
inline double std_stod(const std::string& string)
{ return std::stod(string); }

template<typename T>
concept UnsignedNumber = requires
{ std::is_unsigned_v<std::decay_t<T>>; };

template<typename T>
concept SignedNumber = requires
{ std::is_signed_v<std::decay_t<T>>; };

template<typename T>
concept RealNumber = requires
{ std::is_floating_point_v<std::decay_t<T>>; };

// TODO: Expand to all glm containers (if necessary)
template<typename T>
concept GLMContainer = requires
{
    (
        std::is_same_v<std::decay_t<glm::vec2>, std::decay_t<T>> ||
        std::is_same_v<std::decay_t<glm::vec3>, std::decay_t<T>> ||
        std::is_same_v<std::decay_t<glm::quat>, std::decay_t<T>>
    ) == true;
};

template<typename T, typename N>
bool InterpretNumber(N (*conversion_function)(const std::string&), T& variable, const std::string& string)
{
    T new_value = 0;
    try
    { new_value = conversion_function(string); }
    catch(std::invalid_argument const& exception)
    { return false; }
    variable = new_value;
    return true;
}

template<typename T>
bool StringToNum(T& number, const std::string& string)
{
    if constexpr(std::is_signed_v<T> && std::is_integral_v<T>)
        { return InterpretNumber<T, long>(&std_stol, number, string); }
    else if constexpr(std::is_unsigned_v<T> && std::is_integral_v<T>)
        { return InterpretNumber<T, unsigned long>(&std_stoul, number, string); }
    else if constexpr(std::is_floating_point_v<T>)
        { return InterpretNumber<T, double>(&std_stod, number, string); }
    return false;
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
    if(!StringToNum(output[size - 1], buffer))
        { return false; }
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
inline bool StringToNum(glm::quat& variable, const std::string& string)
{ return InterpretGLM<glm::quat, 4>(variable, string); }

#endif // STRING_TO_NUM_H
