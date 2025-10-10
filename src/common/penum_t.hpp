#ifndef PENUM_T_H
#define PENUM_T_H

#include <format>

struct penum_t
{
    constexpr penum_t(unsigned short id = 0, const char* name = "penum_t"):
        id_(id),
        name_(name)
    {}

    unsigned short id() const { return id_; }
    const char* name()  const { return name_; }

    constexpr operator int() const { return id_; }
    constexpr operator const char*() const { return name_; }
    constexpr operator std::string() const { return name_; }

    constexpr bool operator==(const penum_t& other) const { return (id_ == other.id_); }
    constexpr bool operator!=(const penum_t& other) const { return (id_ != other.id_); }
    constexpr bool operator< (const penum_t& other) const { return (id_ <  other.id_); }
    constexpr bool operator> (const penum_t& other) const { return (id_ >  other.id_); }
    constexpr bool operator<=(const penum_t& other) const { return (id_ <= other.id_); }
    constexpr bool operator>=(const penum_t& other) const { return (id_ >= other.id_); }

private:
    unsigned short id_ = 0;
    const char* name_  = "penum_t";
};

template<>
struct std::formatter<penum_t> : std::formatter<const char*>
{
    auto format(const penum_t& data, std::format_context& ctx) const
    { return std::formatter<const char*>::format(data.name(), ctx); };
};

#endif // PENUM_T_H
