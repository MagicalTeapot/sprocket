#pragma once
#include <string>
#include <memory>

namespace Sprocket {
namespace Audio {

struct MusicImpl;

class Music
{
    std::shared_ptr<MusicImpl> d_impl;

public:
    Music();
    bool Load(const std::string& filename);
    
    void Play() const;
    void Pause() const;
    void Stop() const;
};

}
}