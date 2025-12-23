#ifdef FWD_DCL
    struct ID;
    constexpr unsigned int g_cInvalidID{static_cast<unsigned int>(-1)};
#elif !defined ID_H
#define ID_H

#include "farg.hpp"
#include "common/string_hash.hpp"

struct ID
{
public:
    constexpr ID() noexcept = default;

    constexpr ID(Farg<ID> inID) noexcept:
        id_{inID.id_} {}

    constexpr ID(uint inID, Farg<std::string> inName = "") noexcept:
        id_{inID} {}

    constexpr ID(Farg<std::string> inHashable) noexcept:
        id_{ConstexprHash(inHashable)} {}

    constexpr ID(const char* inHashable) noexcept:
        id_{ConstexprHash(inHashable)} {}

    constexpr ~ID() noexcept = default;

    constexpr explicit operator uint() const { return id_; }

    constexpr uint operator[]() const { return id_;            }
    constexpr bool    invalid() const { return id_ == Invalid; }
    constexpr uint         id() const { return id_;            }

    constexpr bool operator<(Farg<ID> other) const noexcept
    { return id_ < other.id_; }
    constexpr bool operator==(Farg<ID> other) const noexcept
    { return id_ == other.id_; }
    constexpr bool operator!=(Farg<ID> other) const noexcept
    { return id_ != other.id_; }
    constexpr bool operator==(uint id) const noexcept
    { return id_ == id; }
    constexpr bool operator!=(uint id) const noexcept
    { return id_ != id; }

    static constexpr uint Invalid {static_cast<uint>(-1)}; // Same as `UINT_MAX`
    static constexpr uint front   {0};
    static constexpr uint back    {Invalid - 1};

    static uint Generate();

protected:
    uint id_{Invalid};
};

// Pretty-ID: stores a string name that can be accessed via functions.
struct PID : public ID
{
public:
    PID() noexcept;
    PID(uint inID, Farg<std::string> inName = "N/A") noexcept;
    PID(Farg<std::string> inHashable) noexcept;
    PID(const char* inHashable) noexcept;

    virtual ~PID() noexcept;

    virtual Farg<std::string> name() const;
    virtual const char*     c_name() const;
    virtual std::string        log() const;

protected:
    std::string name_{"N/A"};
};

template<typename T>
    concept ID_t = std::derived_from<T, ID>;

template<typename T>
    concept ID_Adjacent = ID_t<T> || std::constructible_from<ID,T>;

template<ID_t T>
    constexpr bool operator==(uint lhs, Farg<T> rhs) noexcept
    { return rhs == lhs; }

template<ID_t T>
    struct std::hash<T>
    {
        size_t operator()(Farg<T> inID) const noexcept
        { return static_cast<size_t>(inID[]); }
    };

#endif // ID_H
