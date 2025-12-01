#ifndef ID_H
#define ID_H

#include "embedded/names.hpp"
#include "common/concepts.hpp"
#include "common/hash.hpp"
#include "frozen/map.h"

#include <map>
#include <string>
#include <format>

#define ConstexprID constexpr ConstexprID_t

struct base_id
{
public:
    constexpr base_id() noexcept = default;
    constexpr base_id(id_t inID)                     noexcept: id_{inID}                      {}
    constexpr base_id(const char* inHashable)        noexcept: id_{ConstexprHash(inHashable)} {}
    constexpr base_id(const std::string& inHashable) noexcept: id_{ConstexprHash(inHashable)} {}

    constexpr ~base_id() noexcept {}

    constexpr id_t operator()() const { return id_; }
    constexpr operator id_t()   const { return id_; }
    constexpr bool  invalid()   const { return id_ == Invalid; }

    virtual const std::string& name() const { return s_cEmpty;        }
    virtual const char*      c_name() const { return s_cEmpty.data(); }

    std::string log() const noexcept
    { return std::format("id {}#{}", (id_ == Invalid) ? "Invalid" : name(), id_); }

    static constexpr id_t Invalid {static_cast<id_t>(-1)}; // Same as `UINT_MAX`
    static constexpr id_t front   {0};
    static constexpr id_t back    {Invalid - 1};

    static id_t Generate();

protected:
    id_t id_{Invalid};

private:
    inline static const std::string s_cEmpty{""};
};

struct ConstexprID_t : public base_id
{
public:
    consteval ConstexprID_t() noexcept = default;
    consteval ConstexprID_t(id_t inID)                     noexcept: base_id{inID}                      {}
    consteval ConstexprID_t(const char* inHashable)        noexcept: base_id{ConstexprHash(inHashable)} {}
    consteval ConstexprID_t(const std::string& inHashable) noexcept: base_id{ConstexprHash(inHashable)} {}

    constexpr ~ConstexprID_t() noexcept {}

    template<typename T> requires is_similar<T, ConstexprID_t>
        consteval ConstexprID_t operator+(const T& Other) const { return ConstexprID_t{id_ + Other}; }
};

struct ID final : public base_id
{
    ID() noexcept = default;

    ID(id_t inID, const std::string& inName = ""):
        base_id{inID}
    { m_pName = new std::string{inName}; }

    ID(const std::string& inHashable) noexcept:
        base_id{inHashable}
    { m_pName = new std::string{inHashable}; }

    ID(const char* inHashable) noexcept:
        base_id{inHashable}
    { m_pName = new std::string{inHashable}; }

    ~ID() noexcept
    { delete m_pName; }

    const std::string&   name() const final { return *m_pName;        }
    const char*        c_name() const final { return m_pName->data(); }

    template<typename T> requires is_similar<T, ID>
        ID operator+(const T& Other) const { return ID{id_ + Other, *m_pName}; }

private:
    std::string m_pName{nullptr};
};

#define ID_FORMATTER(TYPE, FORMATTER, RETURN) \
template<> \
struct std::formatter<TYPE> : std::formatter<FORMATTER> \
{ \
    auto format(TYPE id, std::format_context& ctx) const \
    { return std::formatter<FORMATTER>::format(RETURN, ctx); } \
};

ID_FORMATTER(base_id*, std::string, id->log())
ID_FORMATTER(ID, base_id*, &id)
ID_FORMATTER(ConstexprID_t, base_id*, &id)

#undef ID_FORMATTER

namespace UniqueID
{
    extern id_t Generate();
    extern bool PopLast();
    extern bool Contains(const ID&);
    extern bool Erase(const ID&);
    extern bool Push(const ID&);
    extern void Clear();
    extern bool IsReserved(const ID&);
    extern bool GetReservedID(const std::string& inName, ID& outUID);
    extern bool GetReservedName(const ID& inUID, std::string& outName);

    // Reserved UIDs
    namespace Reserved
    {
        ConstexprID Player{0x1};
        // Images
        ConstexprID i_Missing{0x2};
        ConstexprID i_LightDebug{0x3};
        ConstexprID i_COMP04_5{0x4};
        ConstexprID i_LolBit{0x5};
        // Models
        ConstexprID m_Error{0x6};
        ConstexprID m_Cube{0x7};
        ConstexprID m_Ramiel{0x8};
        // Fonts
        ConstexprID f_Verdana{0x9};
        ConstexprID f_DejaVuSans{0xA};
        ConstexprID f_Audiowide{0xB};
        // Boundaries
        ConstexprID back{f_Audiowide};
        ConstexprID front{Player};

        constexpr frozen::map<ConstexprID_t, std::string, 10>
        EmbeddedResourceNames =
        {
            {     i_Missing, Images::Name::Missing    },
            {  i_LightDebug, Images::Name::LightDebug },
            {    i_COMP04_5, Images::Name::COMP04_5   },
            {      i_LolBit, Images::Name::LolBit     },
            {       m_Error, Models::Name::Error      },
            {        m_Cube, Models::Name::Cube       },
            {      m_Ramiel, Models::Name::Ramiel     },
            {   f_Audiowide, Fonts::Name::Audiowide   },
            {  f_DejaVuSans, Fonts::Name::DejaVuSans  },
            {     f_Verdana, Fonts::Name::Verdana     },
        };
    };

    // Boundaries
    ConstexprID back{ID::Invalid};
    ConstexprID front{Reserved::back + 1};
};

namespace ViewportID
{
    const ID Window{"Window"};
    const ID Editor3DViewport1{"Editor 3D Viewport 1"};
}

#endif // ID_H
