#ifndef PENUM_T_H
#define PENUM_T_H

#include <format>

struct penum_t
{
    constexpr penum_t(unsigned short id = 0, const char* name = "penum_t"):
        mID(id),
        mName(name)
    {}

    unsigned short id() const { return mID; }
    const char* name() const  { return mName; }

    constexpr bool operator==(const penum_t& other) const { return (mID == other.mID); }
    constexpr bool operator!=(const penum_t& other) const { return (mID != other.mID); }
    constexpr bool operator< (const penum_t& other) const { return (mID <  other.mID); }
    constexpr bool operator> (const penum_t& other) const { return (mID >  other.mID); }
    constexpr bool operator<=(const penum_t& other) const { return (mID <= other.mID); }
    constexpr bool operator>=(const penum_t& other) const { return (mID >= other.mID); }

private:
    unsigned short mID = 0;
    const char* mName  = "penum_t";
};

template<>
struct std::formatter<penum_t> : std::formatter<const char*>
{
    auto format(const penum_t& data, std::format_context& ctx) const
    { return std::formatter<const char*>::format(data.name(), ctx); };
};

#endif // PENUM_T_H
