#include "Font.h"
#include "FontLoader.h"

namespace Sprocket {

FontPackage::FontPackage(const std::string& fntFile,
                         const std::string& texFile)
    : d_atlas(texFile, false)
    , d_glyphs()
    , d_size(0.0f)
{
    auto [glyphs, size] = ParseFntFile(fntFile, d_atlas);
    d_glyphs = glyphs;
    d_size = size;
}

Glyph FontPackage::Get(int id) const
{
    auto it = d_glyphs.find(id);
    if (it == d_glyphs.end()) {
        return d_glyphs.find(0)->second;
    }
    return it->second;
}
    
}