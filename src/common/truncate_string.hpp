#ifndef TRUNCATE_STRING_H
#define TRUNCATE_STRING_H

#include <string>

// Initial code taken from: 'https://stackoverflow.com/a/19590797'

inline bool gTruncateString(std::string& output, size_t width = 127, bool show_ellipsis = true)
{
    if(output.length() > width)
    {
        output = output.substr(0, width);
        if(show_ellipsis)
            { output += "..."; }
        return true;
    }
    return false;
}

inline std::string gTruncateString(const std::string& input, size_t width = 127, bool show_ellipsis = true)
{
    std::string output = input;
    gTruncateString(output, width, show_ellipsis);
    return output;
}

#endif // TRUNCATE_STRING_H
