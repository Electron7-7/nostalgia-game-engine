#ifndef THING_H
#define THING_H

typedef unsigned int tid_t;

#include "types/typenames.hpp"

#include <string>

class Thing
{
public:
    virtual ~Thing() = default;

    tid_t GetID() const;
    size_t GetType() const;
    const std::string& GetName() const;

    static constexpr tid_t NoRID = 0;

private:
    friend class TheatreManager;

    tid_t m_ID = Thing::NoRID;
    size_t m_Type = Type::Thing;
    std::string m_Name = "Untitled Thing";
};

template<typename T>
concept IsThing = requires
{ std::derived_from<T, Thing> == true; };

#endif // THING_H
