#pragma once
#include "ECS.h"
#include "EntitySystem.h"

#include <memory>

#include <glm/glm.hpp>

namespace Sprocket {

struct PhysicsEngine3DImpl;

struct CollisionEvent
{
    ecs::Entity entity1;
    ecs::Entity entity2;
    CollisionEvent(ecs::Entity e1, ecs::Entity e2) : entity1(e1), entity2(e2) {}
};

class PhysicsEngine3D : public EntitySystem
{
    static constexpr float TIME_STEP = 1.0f / 120.0f;

    std::unique_ptr<PhysicsEngine3DImpl> d_impl;

public:
    PhysicsEngine3D(const glm::vec3& gravity = {0.0f, -9.81f, 0.0f});
    ~PhysicsEngine3D() = default;

    void on_startup(ecs::Registry& registry, ev::Dispatcher& dispatcher) override;
    void on_update(ecs::Registry& registry, const ev::Dispatcher& dispatcher, double dt) override;

    ecs::Entity Raycast(const glm::vec3& base, const glm::vec3& direction);
        // Given a position in the world and a direction from that point,
        // return a pointer to the entity that it hits, or null if it
        // does not.
};

}