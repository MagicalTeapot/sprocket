#include "ParticleSystem.h"
#include "Components.h"
#include "Random.h"

namespace Sprocket {

ParticleSystem::ParticleSystem(ParticleManager* manager)
    : d_manager(manager)
{
}

void ParticleSystem::OnUpdate(Scene& scene, double dt)
{
    scene.Each<TransformComponent, ParticleComponent>([&](Entity& entity) {
        auto& tc = entity.Get<TransformComponent>();
        auto& pc = entity.Get<ParticleComponent>();

        pc.accumulator += dt;
        while (pc.accumulator > pc.interval) {
            float n = pc.velocityNoise;

            float r = Random<float>(0, pc.velocityNoise);
            float phi = Random<float>(0, 3.142);
            float theta = Random<float>(0, 6.284);
            Maths::vec3 noise = {r * std::sin(theta) * std::cos(phi), r * std::sin(theta) * std::sin(phi), r * std::cos(theta)};
            
            Particle p;
            p.position = tc.position;
            p.velocity = pc.velocity + noise;
            p.acceleration = pc.acceleration;
            p.life = pc.life;
            p.scale = pc.scale;
            d_manager->Emit(p);

            pc.accumulator -= pc.interval;
        }
    });
}

}