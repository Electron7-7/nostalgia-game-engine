#ifndef STRING_TRANSFORM_H
#define STRING_TRANSFORM_H

#include "common/string_concepts.hpp"

#include <format>

// https://stackoverflow.com/a/313990
template<StringContainer T>
constexpr void gSetLowercase(T& ioString)
{
    std::transform(ioString.begin(), ioString.end(), ioString.begin(),
        [](unsigned char character){ return std::tolower(character); });
}

template<StringContainer T>
constexpr std::string gGetLowercase(T inString)
{
    gSetLowercase(inString);
    return inString;
}

template<StringContainer T>
T gTruncateString(const T& input, bool show_ellipsis = false, uint width = 127)
{
    return std::format("{}{}",
        input.substr(0, width),
        (show_ellipsis) ? "..." : "");
}

#endif // STRING_TRANSFORM_H
