#ifndef PENUM_T_H
#define PENUM_T_H

#include <format>

struct penum_t
{
    constexpr penum_t() = default;
    constexpr penum_t(ushort id, const char* name):
        id_{id},
        name_{name} {}

    constexpr void set(const char* name, ushort id)
    { name_ = name; id_ = id; }

    constexpr ushort id() const { return id_; }
    constexpr const char* name()  const { return name_; }

    constexpr operator ushort() const { return id_; }

    constexpr bool operator==(const penum_t& other) const { return (id_ == other.id_); }
    constexpr bool operator< (const penum_t& other) const { return (id_ < other.id_); }

private:
    ushort id_{static_cast<ushort>(-1)};
    const char* name_{""};
};

template<>
struct std::formatter<penum_t> : std::formatter<const char*>
{
    auto format(const penum_t& data, std::format_context& ctx) const
    { return std::formatter<const char*>::format(data.name(), ctx); };
};

#endif // PENUM_T_H
