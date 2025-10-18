#ifndef COMPARE_STRINGS_H
#define COMPARE_STRINGS_H

constexpr bool CompareStrings(const char* Left, const char* Right)
{
    if(Left == Right)
        { return true; }
    else if(Left == nullptr || Right == nullptr)
        { return false; }
    while(*Left != '\0' && *Right != '\0')
    {
        if(*Left != *Right) { return false; }
        ++Left;
        ++Right;
    }
    return (*Left == '\0' && *Right == '\0');
}

#endif // COMPARE_STRINGS_H
