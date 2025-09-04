#include "engine_reference.hpp"
#include "filesystem/binary_file_data.hpp"
#include "resources/engine/fonts.hpp"
#include "resources/engine/images.hpp"
#include "resources/engine/opengl_shaders.hpp"
#include "printing.hpp"

#include <cassert>
#include <map>
#include <any>

static const BinaryFileData s_VerdanaFont(font_TTF_Verdana);
static const BinaryFileData s_AudiowideFont(font_TTF_AudiowideRegular);
static const BinaryFileData s_DejaVuSansFont(font_TTF_DejaVuSansMono);
static const BinaryFileData s_MissingTexture(image_JPG_MISSINGTEXTURE);
static const BinaryFileData s_DoomTexture(image_PNG_COMP045);
static const std::string s_BlinnPhongShaderFrag(glsl_FRAG_BlinnPhong);
static const std::string s_BlinnPhongShaderVert(glsl_VERT_BlinnPhong);

static std::map<std::string, std::any> s_EngineReferences =
{
    { "font_TTF_Verdana",          &s_VerdanaFont          },
    { "font_TTF_AudiowideRegular", &s_AudiowideFont        },
    { "font_TTF_DejaVuSansMono",   &s_DejaVuSansFont       },
    { "image_JPG_MISSINGTEXTURE",  &s_MissingTexture       },
    { "image_PNG_COMP045",         &s_DoomTexture          },
    { "glsl_FRAG_BlinnPhong",      &s_BlinnPhongShaderFrag },
    { "glsl_VERT_BlinnPhong",      &s_BlinnPhongShaderVert },
};

bool IsEngineReference(const std::string& reference)
{ return(s_EngineReferences.contains(reference)); }

bool try_AddEngineReference(const std::string& reference, std::any value)
{
    if(IsEngineReference(reference))
    {
        PRINT_ERROR("try_AddEngineReference - Reference '{}' already exists!", reference)
        return false;
    }

    s_EngineReferences[reference] = value;
    return true;
}

SafeReturn<std::any> try_GetEngineReference(const std::string& reference)
{
    if(!IsEngineReference(reference))
        { return SafeReturn(std::any(), Status::EngineReferenceINVALID_REFERENCE); }

    return s_EngineReferences.at(reference);
}
