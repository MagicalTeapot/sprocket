#pragma once
#include "Core/Window.h"
#include "Events/Event.h"
#include "Utility/Maths.h"
#include "Camera.h"

namespace Sprocket {

class FirstPersonCamera : public Camera
{
    float       d_sensitivity;
        // Mouse sensitivity

public:
    FirstPersonCamera();

    virtual void update(Window* window) override;
};

}