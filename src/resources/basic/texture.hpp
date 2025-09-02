#ifndef TEXTURE_H
#define TEXTURE_H

#include "../resource.hpp"

#include <vector>

struct TextureData
{
    TextureData(unsigned char* Data, unsigned int Size);

    unsigned char* GetData() const;
    unsigned int   GetSize() const;

private:
    unsigned char* m_Data = 0;
    unsigned int   m_Size = 0;
};

struct __texture_t : public Resource
{
protected:
    friend class ResourceManager;
    std::vector<TextureData> m_Data = {};
};

struct Texture : public __texture_t
{
public:
    Texture(const TextureData& Data);

    void LoadData(unsigned char* Data, unsigned int Size);
    const TextureData& GetData() const;

    static const Texture Missing;
};

// TODO: Make more robust, obviously
struct MultiTexture : public __texture_t
{
public:
    MultiTexture(const std::vector<TextureData>& Data);

    void LoadData(std::vector<unsigned char*> Data, std::vector<unsigned int> Sizes);
    const std::vector<TextureData>& GetData() const;

    static const MultiTexture Missing;
};

#endif // TEXTURE_H
