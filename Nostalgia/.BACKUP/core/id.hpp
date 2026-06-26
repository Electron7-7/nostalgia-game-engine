#ifndef ID_H
#define ID_H

#define __id_operator(OPERATION, TYPE, COMPARE_TO...) \
    constexpr bool operator OPERATION(TYPE inOther) const noexcept \
    { return id_ OPERATION inOther COMPARE_TO; }

#define ID_UINT_OPERATOR(OP) __id_operator(OP, uint)
#define ID_ID_OPERATOR(OP)   __id_operator(OP, Farg<ID>, .id_)
#define PID_PID_OPERATOR(OP) __id_operator(OP, const PID&, ())

struct ID
{
public:
    constexpr ID() noexcept = default;

    constexpr ID(Farg<ID> inID) noexcept:
        id_{inID.id_} {}

    constexpr ID(uint inID, Sarg inName = "") noexcept:
        id_{inID} {}

    // Used by the physics engine when converting between a Body's UserData and a Collider's UID
    explicit constexpr ID(uint64 inID) noexcept:
        id_{(inID > static_cast<uint64>(UINT32_MAX)) ? ID::Invalid : static_cast<uint32>(inID)} {}

    constexpr ID(Sarg inHashable) noexcept:
        id_{ConstexprHash(inHashable)} {}

    constexpr ~ID() noexcept = default;

    constexpr explicit operator uint32() const { return id_; }

    constexpr uint operator()() const    { return id_;            }
    constexpr bool    invalid() const    { return id_ == Invalid; }
    constexpr uint         id() const    { return id_;            }

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
    uint id_{ID::Invalid};
};

// Pretty-ID: stores a string name that can be accessed via functions.
struct PID : public ID
{
public:
    static const PID Invalid;

    PID() noexcept;
    PID(uint inID, Sarg inName) noexcept;
    PID(Sarg inHashable) noexcept;
    PID(const char* inHashable) noexcept;

    virtual ~PID() noexcept;

    virtual Sarg          name() const;
    virtual const char* c_name() const;
    virtual std::string    log() const;

    bool operator==(const PID& inOther) const noexcept
    { return id_ == inOther.id_ and not name_.compare(inOther.name_); }

    bool operator!=(const PID& inOther) const noexcept
    { return not (*this == inOther); }

    PID_PID_OPERATOR(<)
    PID_PID_OPERATOR(<=)
    PID_PID_OPERATOR(>)
    PID_PID_OPERATOR(>=)

    ID_UINT_OPERATOR(==)
    ID_UINT_OPERATOR(!=)

    constexpr bool operator==(Farg<ID> inID) const noexcept
    { return id_ == inID(); }

    constexpr bool operator==(Sarg inName) const noexcept
    { return !name_.compare(inName); }

protected:
    std::string name_{"N/A"};
};

template<typename T>
    concept ID_t = std::derived_from<T, ID>;

template<typename T>
    concept ID_Adjacent = ID_t<T> || std::constructible_from<ID,T>;

template<typename T>
    concept ID_or_PID = std::same_as<T, ID> or std::same_as<T, PID>;

constexpr bool operator==(unsigned int lhs, ID rhs) noexcept
{ return rhs == lhs; }

template<ID_t T>
    struct std::hash<T>
    {
        size_t operator()(Farg<T> inID) const noexcept
        { return static_cast<size_t>(inID()); }
    };

using IdSet_t      = std::unordered_set<ID>;
using IdSet_arg    = const IdSet_t&;
using IdSetRev_t   = std::unordered_set<ID, std::greater<ID>>;
using IdSetRev_arg = const IdSetRev_t&;
using IdVec_t      = std::vector<ID>;
using IdVec_arg    = const IdVec_t&;

using FPID          = const PID&;
using PidSet_t      = std::unordered_set<PID>;
using PidSet_arg    = const PidSet_t&;
using PidSetRev_t   = std::unordered_set<PID, std::greater<PID>>;
using PidSetRev_arg = const PidSetRev_t&;
using PidVec_t      = std::vector<PID>;
using PidVec_arg    = const PidVec_t&;

template<ID_or_PID T>
    using IdSet       = std::unordered_set<T>;
template<ID_or_PID T>
    using FIdSet      = const IdSet<T>&;
template<ID_or_PID T>
    using IdSetRev    = std::unordered_set<T, std::greater<T>>;
template<ID_or_PID T>
    using FIdSetRev   = const IdSetRev<T>&;
template<ID_or_PID T>
    using IdVec       = std::vector<T>;
template<ID_or_PID T>
    using FIdVec      = const IdVec<T>&;
template<ID_or_PID T, typename U>
    using IdMap       = std::unordered_map<T, U>;
template<ID_or_PID T, typename U>
    using FIdMap      = const IdMap<T, U>&;

#undef __id_operator
#undef ID_UINT_OPERATOR
#undef ID_ID_OPERATOR

#endif // ID_H
