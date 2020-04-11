#include "Texture.h"
#include "Log.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Sprocket {

Texture::Texture(const std::string& pngFile)
    : d_texture(std::make_shared<TEX>())
{
    glBindTexture(GL_TEXTURE_2D, d_texture->value());

    SPKT_LOG_INFO("Loading texture '{}'", pngFile);
    unsigned char* data = stbi_load(
        pngFile.c_str(),
        &d_width, &d_height, &d_bpp, 4);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D,
                 0, GL_RGBA8, d_width, d_height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(int width, int height, const std::vector<unsigned char>& data)
    : d_texture(std::make_shared<TEX>())
    , d_width(width)
    , d_height(height)
    , d_bpp(4)
{
    glBindTexture(GL_TEXTURE_2D, d_texture->value());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D,
                 0, GL_RGBA, d_width, d_height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture()
    : d_texture(Texture::white().d_texture)
    , d_width(Texture::white().d_width)
    , d_height(Texture::white().d_height)
    , d_bpp(Texture::white().d_bpp)
{
}

void Texture::bind(int slot) const
{
    d_slot = slot;
    glBindTextureUnit(slot, d_texture->value());
}

void Texture::unbind() const
{
    glBindTextureUnit(d_slot, 0);
    d_slot = -1;
}

Texture Texture::white()
{
    static const Texture white(1, 1, {0xff, 0xff, 0xff, 0x55});
    return white;
}

bool Texture::operator==(const Texture& other) const
{
    return d_texture->value() == other.d_texture->value();
}

}