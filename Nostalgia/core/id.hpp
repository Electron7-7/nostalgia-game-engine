#ifndef ID_H
#define ID_H

#ifdef NOSTALGIA_GCC_PCH
#   include <Nostalgia/Nostalgia.hpp>
#endif

#define __id_operator(OPERATION, TYPE, COMPARE_TO...) \
    constexpr bool operator OPERATION(TYPE inOther) const noexcept \
    { return id_ OPERATION inOther COMPARE_TO; }

#define ID_UINT_OPERATOR(OP) __id_operator(OP, uint)
#define ID_ID_OPERATOR(OP)   __id_operator(OP, Farg<ID>, .id_)
#define PID_PID_OPERATOR(OP) __id_operator(OP, Farg<PID>, [])

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

    constexpr ~ID() noexcept = default;

    constexpr explicit operator uint() const { return id_; }

    constexpr uint operator[]() const { return id_;            }
    constexpr bool    invalid() const { return id_ == Invalid; }
    constexpr uint         id() const { return id_;            }

    ID_ID_OPERATOR(==)
    ID_ID_OPERATOR(!=)
    ID_ID_OPERATOR(<)
    ID_ID_OPERATOR(<=)
    ID_ID_OPERATOR(>)
    ID_ID_OPERATOR(>=)

    ID_UINT_OPERATOR(==)
    ID_UINT_OPERATOR(!=)

    static constexpr uint Invalid {static_cast<uint>(-1)}; // Same as `UINT_MAX`
    static constexpr uint front   {0};
    static constexpr uint back    {Invalid - 1};

protected:
    uint id_{Invalid};
};

// Pretty-ID: stores a string name that can be accessed via functions.
struct PID : public ID
{
public:
    PID() noexcept;
    PID(uint inID, Farg<std::string> inName) noexcept;
    PID(Farg<std::string> inHashable) noexcept;
    PID(const char* inHashable) noexcept;

    virtual ~PID() noexcept;

    virtual Farg<std::string> name() const;
    virtual const char*     c_name() const;
    virtual std::string        log() const;

    PID_PID_OPERATOR(==)
    PID_PID_OPERATOR(!=)
    PID_PID_OPERATOR(<)
    PID_PID_OPERATOR(<=)
    PID_PID_OPERATOR(>)
    PID_PID_OPERATOR(>=)

    ID_UINT_OPERATOR(==)
    ID_UINT_OPERATOR(!=)

    constexpr bool operator==(Farg<ID> inID) const noexcept
    { return id_ == inID[]; }

    constexpr bool operator==(Sarg inName) const noexcept
    { return !name_.compare(inName); }

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

using FPID      = const PID&;
using IdSet_t   = std::set<ID>;
using IdSet_arg = const IdSet_t&;
using IdSetRev_t   = std::set<ID, std::greater<ID>>;
using IdSetRev_arg = const IdSetRev_t&;
using IdVec_t   = std::vector<ID>;
using IdVec_arg = const IdVec_t&;

#undef __id_operator
#undef ID_UINT_OPERATOR
#undef ID_ID_OPERATOR

#endif // ID_H
