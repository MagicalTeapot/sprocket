#pragma once
#include "Maths.h"
#include "Window.h"

namespace Sprocket {

class Camera
{
protected:
    Maths::vec3 d_position;

    Maths::vec3 d_direction;
        // Vector that points in the direction of the camera. This
        // should ideally be a unit vector.

    float d_pitch;
    float d_yaw;
    float d_roll;
        // Euler angles. These could be derived from the direction
        // and so should be kepy in sync.

    float d_fov;
    float d_nearPlane;
    float d_farPlane;
        // Projection matrix attributes.

public:
    Camera();

    virtual void update(Window* window) = 0;

    Maths::vec3 position() const { return d_position; }
    Maths::vec3 direction() const { return d_direction; }

    float pitch() const { return d_pitch; }
    float yaw() const { return d_yaw; }
    float roll() const { return d_roll; }

    float fov() const { return d_fov; }
    float nearPlane() const { return d_nearPlane; }
    float farPlane() const { return d_farPlane; }
};

}