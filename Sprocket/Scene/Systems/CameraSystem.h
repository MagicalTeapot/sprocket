#pragma once
#include "EntitySystem.h"

namespace Sprocket {

class CameraSystem : public EntitySystem
{
    float d_aspectRatio;

public:
    CameraSystem(float aspectRatio);
    void OnStartup(Scene& scene) override;
    void OnEvent(Scene& scene, Event& event) override;
};

}