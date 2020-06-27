#include "Sound.h"

#include <SFML/Audio.hpp>

namespace Sprocket {
namespace Audio {

struct SoundImpl
{
    sf::SoundBuffer buffer;
};

Sound::Sound()
    : d_impl(std::make_shared<SoundImpl>())
{
}

bool Sound::Load(const std::string& filename)
{
    return d_impl->buffer.loadFromFile(filename);
}

struct SourceImpl
{
    sf::Sound sound;
}; 

Source::Source()
    : d_impl(std::make_shared<SourceImpl>())
{
}


void Source::SetSound(const Sound& sound)
{
    d_impl->sound.setBuffer(sound.d_impl->buffer);
}

void Source::Play() const
{
    d_impl->sound.play();
}

void Source::Pause() const
{
    d_impl->sound.pause();
}

void Source::Stop() const
{
    d_impl->sound.stop();
}

}
}