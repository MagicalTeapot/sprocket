#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <cstddef>
#include <memory>
#include <vector>
#include <string>

#include "texture-atlas.h"

namespace Sprocket {

enum class RenderMode
{
    RENDER_NORMAL,
    RENDER_OUTLINE_EDGE,
    RENDER_OUTLINE_POSITIVE,
    RENDER_OUTLINE_NEGATIVE
};

struct Kerning
{
    uint32_t codepoint;
    float kerning;
};

struct TextureGlyph
{
    uint32_t codepoint;

    std::size_t width;
    std::size_t height;

    int offset_x;
    int offset_y;

    float advance_x;
    float advance_y;

    float s0;
    float t0;
    float s1;
    float t1;

    std::vector<Kerning> kerning;

    RenderMode rendermode;

    float outline_thickness;
};

struct texture_font_t
{
    std::vector<std::shared_ptr<TextureGlyph>> glyphs;
    std::shared_ptr<FontAtlas> atlas;
    std::string filename;
    float size;
    int hinting;
    RenderMode rendermode;
    float outline_thickness;
    float height;
    float underline_position;
    float underline_thickness;
    int padding;
};


std::shared_ptr<texture_font_t>
texture_font_new_from_file(std::shared_ptr<FontAtlas> atlas,
                           const float pt_size,
                           const char* filename );

std::shared_ptr<TextureGlyph>
texture_font_get_glyph(std::shared_ptr<texture_font_t> self,
                       const char* codepoint );

std::shared_ptr<TextureGlyph>
texture_font_find_glyph(std::shared_ptr<texture_font_t> self,
                        const char* codepoint );

int texture_font_load_glyph(std::shared_ptr<texture_font_t> self,
                            const char* codepoint );

float texture_glyph_get_kerning(const std::shared_ptr<TextureGlyph> self,
                                const char* codepoint );

std::shared_ptr<TextureGlyph> texture_glyph_new();

}
