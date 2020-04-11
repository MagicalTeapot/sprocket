#pragma once
#include "Texture.h"

namespace Sprocket {
    
struct Material
{
    Texture texture;
    float   shineDamper  = 1.0f;
    float   reflectivity = 0.0f;
};

bool operator==(const Material& lhs, const Material& rhs);

}