#pragma once
#include <Sprocket/Scene/ecs.h>

namespace anvil {

inline void delete_below_50_system(spkt::registry& registry, double)
{
    registry.destroy_if<spkt::Transform3DComponent>([&](spkt::entity entity) {
        const auto& t = registry.get<spkt::Transform3DComponent>(entity);
        return t.position.y < -50.0f;
    });
}

inline void animation_system(spkt::registry& registry, double dt)
{
    for (auto [ac] : registry.view_get<spkt::AnimatedModelComponent>()) {
        ac.animation_time += (float)dt * ac.animation_speed;
    }
}

}