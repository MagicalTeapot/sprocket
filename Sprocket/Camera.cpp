#include "Camera.h"
#include "Keyboard.h"

#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>

namespace Sprocket {

Camera::Camera()
    : d_position(glm::vec3(0.0f, 0.0f, 0.0f))
    , d_pitch(0)
    , d_yaw(0)
    , d_roll(0)
{
}

void Camera::move()
{
    if (Keyboard::isKeyDown(Keyboard::W))
    {
        d_position.z += -std::cos(glm::radians(d_yaw)) * 0.02f;
        d_position.x += std::sin(glm::radians(d_yaw)) * 0.02f;
    }
    if (Keyboard::isKeyDown(Keyboard::E))
    {
        d_position.z += std::sin(glm::radians(d_yaw)) * 0.02f;
        d_position.x += std::cos(glm::radians(d_yaw)) * 0.02f;
    }
    if (Keyboard::isKeyDown(Keyboard::Q))
    {
        d_position.z += -std::sin(glm::radians(d_yaw)) * 0.02f;
        d_position.x += -std::cos(glm::radians(d_yaw)) * 0.02f;
    }
    if (Keyboard::isKeyDown(Keyboard::S))
    {
        d_position.z += std::cos(glm::radians(d_yaw)) * 0.02f;
        d_position.x += -std::sin(glm::radians(d_yaw)) * 0.02f;
    }
    if (Keyboard::isKeyDown(Keyboard::A))
    {
        d_yaw -= 1.0f;
    }
    if (Keyboard::isKeyDown(Keyboard::D))
    {
        d_yaw += 1.0f;
    }
    if (Keyboard::isKeyDown(Keyboard::R))
    {
        d_pitch = std::min(std::max(d_pitch - 1.0f, -90.0f), 90.0f);
    }
    if (Keyboard::isKeyDown(Keyboard::F))
    {
        d_pitch = std::min(std::max(d_pitch + 1.0f, -90.0f), 90.0f);
    }
    if (Keyboard::isKeyDown(Keyboard::SPACE))
    {
        d_position.y += 0.02f;
    }
    if (Keyboard::isKeyDown(Keyboard::LSHIFT))
    {
        d_position.y -= 0.02f;
    }
}

}