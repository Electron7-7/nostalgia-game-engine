#ifndef TO_LOWER_H
#define TO_LOWER_H

#include <string>
#include <cctype>
#include <algorithm>

// https://stackoverflow.com/a/313990
static constexpr std::string ToLower(std::string string)
{
    std::transform(string.begin(), string.end(), string.begin(),
        [](unsigned char character){ return std::tolower(character); });
    return string;
}

#endif // TO_LOWER_H
