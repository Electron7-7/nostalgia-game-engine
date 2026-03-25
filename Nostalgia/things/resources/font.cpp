#include "./font.hpp"
#include "rendering/texture_buffer.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace TheatreFile;

void Font::Ready()
{
    Super::Ready();

    FT_Library ft;
    if(FT_Init_FreeType(&ft))
    {
        print_error("Could not init FreeType Library!");
        return;
    }

    FT_Face face;
    if(FT_New_Memory_Face(ft, m_pFont->Data(), m_pFont->Size(), 0, &face))
    {
        print_error("FreeType failed to load font");
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, mFontSize);

    FT_GlyphSlot g = face->glyph;
    for(u_char c{0}; c < 128; ++c)
    {
        if(FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            print_warning("FreeType failed to load Glyph for character '{}'", (char)c);
            continue;
        }

        // https://stackoverflow.com/q/53267905
        auto& glyph{mGlyphs[c] = {TextureBuffer::Create(), c}};

        glyph.bitmap_width  = g->bitmap.width;
        glyph.bitmap_height = g->bitmap.rows;
        glyph.bitmap_left   = g->bitmap_left;
        glyph.bitmap_top    = g->bitmap_top;
        glyph.advance_x     = g->advance.x;
        glyph.advance_y     = g->advance.y;

        glyph.texture->Load(face->glyph->bitmap.buffer,
            {glyph.bitmap_width, glyph.bitmap_height, DATA_FORMAT_RED});
        glyph.texture->SetSamplerState({});
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void Font::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(m_pFont, "Font", "File");
    data.get_variable(mFontSize, "Height", "Size");
}

Shared<ThingData> Font::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(m_pFont, "File");
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
