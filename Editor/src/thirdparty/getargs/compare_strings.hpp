#ifndef COMPARE_STRINGS_H
#define COMPARE_STRINGS_H

constexpr bool CompareStrings(const char* Left, const char* Right)
{
    if(Left == Right) // Hey, it could happen
    { return true; }

    if(Left == nullptr || Right == nullptr) // All my homies HATE segfaults!!!!
    { return false; }

    while(*Left != '\0' && *Right != '\0')
    {
        if(*Left != *Right)
        { return false; }

        Left++;
        Right++;
    }

    if(*Left != '\0' || *Right != '\0') // One of them is shorter than the other!
    { return false; }

    return true;
}

#endif // COMPARE_STRINGS_H
