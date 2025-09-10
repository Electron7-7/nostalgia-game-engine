#ifndef THING_H
#define THING_H

#include "id.hpp"
#include "theatre/theatre_data.hpp"
#include "types/typenames.hpp"

#include <string>

class _thing
{
public:
    virtual ~_thing() = default;

    virtual void SetupVariables(const data_t&) = 0;
    virtual bool Initialize() = 0;
    virtual void Destroy() = 0;

    id_t GetID() const;
    size_t GetType() const;
    const std::string& GetName() const;

private:
    friend class TheatreManager;

    id_t m_ID = NoID;
    size_t m_Type = Type::Thing;
    std::string m_Name = "Untitled Thing";
};

class Thing : public _thing
{
public:
    virtual void SetupVariables(const data_t&) {}
    virtual bool Initialize() { return true; }
    virtual void Destroy() {}
};

template<typename T>
concept IsThing = requires
{ std::derived_from<T, Thing> == true; };

#endif // THING_H
