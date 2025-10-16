#ifndef FORMAT_BOOL_H
#define FORMAT_BOOL_H

namespace fmt
{
    constexpr const char* Bool(bool Bool)
    { return Bool ? "true" : "false"; }

    constexpr const char* BoolAlt(bool Bool)
    { return Bool ? "yes" : "no"; }

    namespace cap
    {
        constexpr const char* Bool(bool Bool)
        { return Bool ? "True" : "False"; }

        constexpr const char* BoolAlt(bool Bool)
        { return Bool ? "Yes" : "No"; }
    }
}


#endif // FORMAT_BOOL_H
