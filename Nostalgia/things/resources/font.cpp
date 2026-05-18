#include "./font.hpp"
#include "things/thing_data.hpp"
#include "things/thing_factory.hpp"
#include "rendering/texture_buffer.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace TheatreFile;

Shared<Font> Font::CreateFromMemory(const uchar* inData, size_t inSize)
{
    Shared<Font> output{DCast<Font>(ThingFactory::MakeThing<Font>("Font"))};
    output->mFontData = {inData, inData + inSize};
    output->LoadFont();
    return output;
}

void Font::InitVariables()
{
    Super::InitVariables();
}

void Font::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mFontSize, "Height", "Size");
    if(data.get_variable(mFilepath, "Font", "File") == OK)
    {
        if(not FileSystem::ResolveFilePath(mFilepath))
        {
            print_warning("Failed to read font file at '{}'", mFilepath);
            return;
        }
        FileSystem::Read(mFilepath, mFontData);
    }
    LoadFont();
}

Shared<ThingData> Font::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(mFilepath, "File",
        TheatreFile::VARIABLE_HINT_FILE, FileSystem::CreateFileExtensionFilter("Font Files", ".ttf,.otf"));
    data->set_variable(mFontSize, "Height");

    return data;
}

Farg<Font::Glyph> Font::GetGlyph(char inCharacter) const
{
    static Glyph empty_glyph{};
    if(auto found_it{mGlyphs.find(inCharacter)}; found_it != mGlyphs.end())
        { return found_it->second; }
    return empty_glyph;
}

Farg<Font::GlyphsMap> Font::GetGlyphs() const
{ return mGlyphs; }

float Font::GetSize() const
{ return mFontSize; }

void Font::LoadFont()
{
    if(mFontData.empty())
    {
        print_warning("No font data found in '{}'", name());
        return;
    }

    FT_Library ft;
    if(FT_Init_FreeType(&ft))
    {
        print_error("Could not init FreeType Library!");
        return;
    }

    FT_Face face;
    if(FT_New_Memory_Face(ft, mFontData.data(), mFontData.size(), 0, &face))
    {
        print_error("FreeType failed to load font");
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, mFontSize);

    FT_GlyphSlot g = face->glyph;
    for(uchar c{0}; c < 128; ++c)
    {
        if(FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            print_warning("FreeType failed to load Glyph for character '{}'", (char)c);
            continue;
        }

        // https://stackoverflow.com/q/53267905
        auto& glyph{mGlyphs[c] =
        {
            TextureBuffer::Create(
                {
                    TEXTURE_TYPE_2D,
                    static_cast<int>((g->bitmap.width) ? g->bitmap.width : 1),
                    static_cast<int>((g->bitmap.rows)  ? g->bitmap.rows  : 1),
                    DATA_FORMAT_RED
                }),
            c
        }};

        glyph.bitmap_width  = g->bitmap.width;
        glyph.bitmap_height = g->bitmap.rows;
        glyph.bitmap_left   = g->bitmap_left;
        glyph.bitmap_top    = g->bitmap_top;
        glyph.advance_x     = g->advance.x;
        glyph.advance_y     = g->advance.y;

        glyph.texture->SetSamplerState({SAMPLER_FILTER_NEAREST, SAMPLER_FILTER_NONE, SAMPLER_FILTER_NEAREST});
        glyph.texture->SetData({face->glyph->bitmap.buffer});
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}
