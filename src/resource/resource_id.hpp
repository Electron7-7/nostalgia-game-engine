#ifndef RESOURCE_ID_H
#define RESOURCE_ID_H

#include <string>

typedef size_t __rid_t; // unsigned long

struct ResourceID
{
public:
    constexpr ResourceID(__rid_t ID, const std::string& Name): m_Name(Name), m_ID(ID), m_Valid(true) {}
    constexpr ResourceID(const std::string& Name, __rid_t ID): m_Name(Name), m_ID(ID), m_Valid(true) {}
    constexpr ResourceID() = default;

    constexpr bool operator==(const ResourceID& Compare) const
    { return (m_ID == Compare.m_ID && (this->m_Valid == Compare.m_Valid)); }
    constexpr bool operator!=(const ResourceID& Compare) const
    { return!(m_ID == Compare.m_ID && (this->m_Valid == Compare.m_Valid)); }
    constexpr bool operator <(const ResourceID& Compare) const
    { return (m_ID < Compare.m_ID); }

    friend constexpr bool operator==(const std::string& Left, const ResourceID& Right)
    { return(!Right.m_Name.compare(Left)); }
    friend constexpr bool operator==(const ResourceID& Left, const std::string& Right)
    { return(!Left.m_Name.compare(Right)); }

private:
    std::string m_Name = "Untitled";
    __rid_t m_ID = 0;
    bool m_Valid = false;
};

namespace RID
{
    static ResourceID meshCube;
    static ResourceID meshQuad;
    static ResourceID meshPyramid;
    static ResourceID meshRamiel;
    // Add to this!
}

#endif // RESOURCE_ID_H
