#ifndef TEXTURE_H
#define TEXTURE_H

#include "../resource.hpp"
#include "filesystem/binary_file_data.hpp"
#include "safe_return.hpp"

class Texture : public Resource
{
public:
    static const Texture Missing;

    virtual void UpdateVariables(const data_t&);

    Texture();
    Texture(const BinaryFileData& Data);

    void LoadTextureFile(const std::string& File);
    void LoadTextureData(const BinaryFileData& Data);

    SafeStatus try_CreateTexture();

    const BinaryFileData& GetData() const;

private:
    friend class TheatreManager;
    ResourceStatus m_TextureDataStatus = ResourceStatus::NOT_PROCESSED;

    BinaryFileData m_Data;
    std::string m_File = "";
};

// TODO: MultiTexture will just be a wrapper for a vector of Textures

#endif // TEXTURE_H
