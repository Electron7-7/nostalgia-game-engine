#ifndef TEXTURE_H
#define TEXTURE_H

#include "resources/resource.hpp"
#include "filesystem/binary_file_data.hpp"
#include "safe_return.hpp"

struct Texture : public Resource
{
public:
    Texture();

    static const Texture Missing;

    void LoadTextureFile(const std::string& File);
    void LoadTextureData(const BinaryFileData& Data);

    SafeStatus try_CreateTexture();

    const BinaryFileData& GetData() const;

private:
    friend class TheatreManager;
    Texture(const BinaryFileData& Data);

    ResourceStatus m_TextureDataStatus = ResourceStatus::NOT_PROCESSED;

    BinaryFileData m_Data;
    std::string m_File = "";
};

// TODO: MultiTexture will just be a wrapper for a vector of Textures

#endif // TEXTURE_H
