#include "texture.hpp"

void Texture::SetVariables(Farg<ThingData> data)
{
    Resource::SetVariables(data);
    // data.GetVariable(format.format, "Format");
    // data.GetVariable(format.type, "Type");
    // data.GetVariable(format.width, "Width");
    // data.GetVariable(format.height, "Height");
    // data.GetVariable(format.depth, "Depth");
    // data.GetVariable(format.mipmaps, "MipMaps");
    // data.GetVariable(format.samples, "Samples");
    // data.GetVariable(format.array_layers, "Array Layers");
    // data.GetVariable(base_mipmap, "Base MipMap");
    // data.GetVariable(base_layer, "Base Array Layer");
    // data.GetVariable(bound_to_framebuffer, "Bound to Framebuffer");

    // g_pBackendManager->Graphics()->BufferTexture(mFileData, mUID);
}

Shared<ThingData> Texture::GetVariables() const
{
    Shared<ThingData> data{Resource::GetVariables()};
    // data->AddVariable(format.format, "Format");
    // data->AddVariable(format.type, "Type");
    // data->AddVariable(format.width, "Width");
    // data->AddVariable(format.height, "Height");
    // data->AddVariable(format.depth, "Depth");
    // data->AddVariable(format.mipmaps, "MipMaps");
    // data->AddVariable(format.samples, "Samples");
    // data->AddVariable(format.array_layers, "Array Layers");
    // data->AddVariable(base_mipmap, "Base MipMap");
    // data->AddVariable(base_layer, "Base Array Layer");
    // data->AddVariable(bound_to_framebuffer, "Bound to Framebuffer");
    return data;
}
