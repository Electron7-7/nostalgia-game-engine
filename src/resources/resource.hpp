#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>

typedef unsigned int rid_t;

// Pre-Embedded RIDs
struct RID
{
    static const rid_t ErrorMesh;
    static const rid_t RamielMesh;
    static const rid_t MissingTexture;
    // static const rid_t RamielTexture;
    static const rid_t DoomTexture;
};

struct Resource
{
public:
    rid_t GetID() const { return m_ID; }
    const std::string& GetName() const { return m_Name; }

private:
    friend class ResourceManager;
    rid_t m_ID;
    std::string m_Name = "UntitledResource";
};

#endif // RESOURCE_H
