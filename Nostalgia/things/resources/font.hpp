#ifndef FONT_H
#define FONT_H

#include <Nostalgia/fwd/rendering.hpp>
#include <Nostalgia/things/resources/resource.hpp>

class Font : public Resource
{
public:
	struct Glyph
    {
        Shared<TextureBuffer> texture{nullptr};
        u_char character{'\0'};
        int bitmap_width{0},
            bitmap_height{0},
            bitmap_left{0},
            bitmap_top{0},
            advance_x{0},
            advance_y{0};

        constexpr bool operator==(Farg<Glyph> rhs) const noexcept
        { return character == rhs.character; }
        constexpr bool operator==(char rhs) const noexcept
        { return character == rhs; }
    };

    using GlyphsMap = std::unordered_map<char, Glyph>;

    static Shared<Font> CreateFromMemory(const uchar* inData, size_t inSize);

    SET_SUPER(Resource)
    SET_TYPEID(ThingType::Font)
    virtual SIG_SET_VARIABLES override;
    virtual SIG_GET_VARIABLES override;

    virtual Farg<Glyph> GetGlyph(char inCharacter) const;
    virtual Farg<GlyphsMap> GetGlyphs() const;
    virtual float GetSize() const;

protected:
    friend class ImGui_Debugger;
    Shared<FileData> m_pFont{MakeShared<FileData>()};
    void* m_pFace{nullptr};
    int mFontSize{48};
    long mFontAscend{0};
    GlyphsMap mGlyphs{};

    void LoadFont();
};

#endif // FONT_H
