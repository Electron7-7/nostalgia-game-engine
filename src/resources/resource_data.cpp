#include "resource_data.hpp"
#include "data/fonts.hpp"
#include "data/images.hpp"
#include "data/models.hpp"
#include "data/opengl_shaders.hpp"
#include "printing.hpp"

#include <map>

static const BinaryFileData s_EmptyBinaryData;
static const StringFileData s_EmptyStringData;

static std::map<std::string, BinaryFileData> s_BinaryResourceData =
{
    { "VerdanaFont",          font_TTF_Verdana          },
    { "AudiowideRegularFont", font_TTF_AudiowideRegular },
    { "DejaVuSansMonoFont",   font_TTF_DejaVuSansMono   },
    { "MissingTexture",       image_JPG_MISSINGTEXTURE  },
    { "DoomTexture",          image_PNG_COMP045         },
};

static std::map<std::string, StringFileData> s_StringResourceData =
{
    { "ErrorModel",  model_OBJ_Error  },
    { "RamielModel", model_OBJ_Ramiel },
    { "BlinnPhongFragShader", glsl_FRAG_BlinnPhong },
    { "BlinnPhongVertShader", glsl_VERT_BlinnPhong },
};

bool Exists(const std::string& name)
{
    return
    (
        s_BinaryResourceData.contains(name) ||
        s_StringResourceData.contains(name)
    );
}

bool ResourceData::try_Add(const std::string& name, const BinaryFileData& data)
{
    if(s_BinaryResourceData.contains(name))
    {
        PRINT_ERROR("ResourceData::try_Add - BinaryFileData, '{}', already exists!", name)
        return false;
    }

    s_BinaryResourceData[name] = data;
    return true;
}

bool ResourceData::try_Add(const std::string& name, const StringFileData& data)
{
    if(s_StringResourceData.contains(name))
    {
        PRINT_ERROR("ResourceData::try_Add - StringFileData, '{}', already exists!", name)
        return false;
    }

    s_StringResourceData[name] = data;
    return true;
}

SafeReturn<const BinaryFileData&> ResourceData::try_GetBinaryData(const std::string& name)
{
    if(!s_BinaryResourceData.contains(name))
        { return SafeReturn<const BinaryFileData&>(s_EmptyBinaryData, Status::EngineReferenceINVALID_REFERENCE); }
    return s_BinaryResourceData.at(name);
}

SafeReturn<const StringFileData&> ResourceData::try_GetStringData(const std::string& name)
{
    if(!s_StringResourceData.contains(name))
        { return SafeReturn<const StringFileData&>(s_EmptyStringData, Status::EngineReferenceINVALID_REFERENCE); }
    return s_StringResourceData.at(name);
}
