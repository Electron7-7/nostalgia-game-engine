#include "texture.hpp"
#include "embedded/images.hpp"

const Texture Texture::Missing({image_JPG_MISSINGTEXTURE, image_JPG_MISSINGTEXTURE_len});
const MultiTexture MultiTexture::Missing({{image_JPG_MISSINGTEXTURE, image_JPG_MISSINGTEXTURE_len}});

TextureData::TextureData(unsigned char* data, unsigned int size)
: m_Data(data), m_Size(size)
{}

Texture::Texture(const TextureData& data)
{ m_Data = {data}; }

void Texture::LoadData(unsigned char* data, unsigned int size)
{
    m_Data.clear();
    m_Data.push_back(TextureData{data, size});
}

const TextureData& Texture::GetData() const
{
    if(m_Data.empty())
    { return Texture::Missing.GetData(); }

    return m_Data.front();
}

MultiTexture::MultiTexture(const std::vector<TextureData>& data)
{ m_Data = data; }

void MultiTexture::LoadData(std::vector<unsigned char*> data, std::vector<unsigned int> sizes)
{
    m_Data.clear();

    size_t number_of_textures = sizes.size();

    for(size_t i = 0 ; i < number_of_textures ; ++i)
    { m_Data.push_back(TextureData{data.at(i), sizes.at(i)}); }
}

const std::vector<TextureData>& MultiTexture::GetData() const
{
    if(m_Data.empty())
    { return MultiTexture::Missing.GetData(); }

    return m_Data;
}
