#pragma once
#include "Widget.h"
#include "Quad.h"
#include "Maths.h"
#include "Texture.h"

#include <string>

namespace Sprocket {

class Image : public Widget
{
    void updateImpl(Window* window) override;
    bool handleEventImpl(Window* window, const Event& event) override;
    void drawImpl(DisplayRenderer* renderer) const override;

public:
    Image(const Texture& texture);
};

}