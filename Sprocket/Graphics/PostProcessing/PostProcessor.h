#pragma once
#include "Effect.h"
#include "Mesh.h"

#include <vector>

namespace Sprocket {

using EffectPipeline = std::vector<std::unique_ptr<Effect>>;

class PostProcessor
{
    int d_width;
    int d_height;
        // Dimensions of the screen.

    EffectPipeline d_effects;
    Mesh d_quad;

    void AddEffect(std::unique_ptr<Effect> effect);

public:
    PostProcessor(int width, int height);

    void Bind() const;
    void Unbind() const;
        // Binds/unbinds the first effect in the pipeline.
        // An error is thrown if no effects have been added.

    void Draw();

    void SetScreenSize(int width, int height);

    template <typename T, typename... Args>
    void AddEffect(Args&&... args) {
        d_effects.push_back(std::make_unique<T>(d_width, d_height, args...));
    }
};

}