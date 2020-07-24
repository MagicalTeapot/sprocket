#include "PathFollower.h"

void PathFollower::OnUpdate(Sprocket::EntityManager& manager, double dt)
{
    using namespace Sprocket;

    manager.Each<TransformComponent, PathComponent>([&](Entity& entity) {
        auto& transform = entity.Get<TransformComponent>();
        auto& path = entity.Get<PathComponent>();
        if (path.markers.empty()) { return; }
        
        Maths::vec3 direction = path.markers.front() - transform.position;
        Maths::Normalise(direction);
        Maths::vec3 advance = path.speed * (float)dt * direction;

        auto MagSq = [](const Maths::vec3& v) {
            return v.x*v.x + v.y*v.y + v.z*v.z;
        };

        if (MagSq(advance) < MagSq(path.markers.front() - transform.position)) {
            transform.position += advance;
        } else {
            transform.position = path.markers.front();
            path.markers.pop();
        }
    });
}