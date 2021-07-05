#include "ParticleSystem.h"
#include "Components.h"
#include "apecs.hpp"
#include "Random.h"
#include "Scene.h"
#include "ParticleManager.h"

namespace spkt {
namespace {

void emit_particle(ParticleSingleton& ps, const particle& p)
{
    ps.particles[ps.next_slot] = p;
    ps.next_slot = --ps.next_slot % NUM_PARTICLES;
}

}

void particle_system_init(apx::registry& registry, ParticleManager* particle_manager)
{
    auto singleton = registry.find<Singleton>();
    auto& ps = registry.emplace<ParticleSingleton>(singleton);
    ps.particle_manager = particle_manager;
}

void particle_system(apx::registry& registry, double dt)
{
    auto& ps = get_singleton<ParticleSingleton>(registry);

    for (auto entity : registry.view<ParticleComponent>()) {
        auto& tc = registry.get<Transform3DComponent>(entity);
        auto& pc = registry.get<ParticleComponent>(entity);

        pc.accumulator += dt;
        while (pc.accumulator > pc.interval) {
            float n = pc.velocityNoise;

            float r = Random<float>(0, pc.velocityNoise);
            float phi = Random<float>(0, 3.142);
            float theta = Random<float>(0, 6.284);
            glm::vec3 noise = {r * std::sin(theta) * std::cos(phi), r * std::sin(theta) * std::sin(phi), r * std::cos(theta)};
            
            Particle p;
            p.position = tc.position;
            p.velocity = pc.velocity + noise;
            p.acceleration = pc.acceleration;
            p.life = pc.life;
            p.scale = pc.scale;
            ps.particle_manager->Emit(p);

            spkt::particle p2;
            p2.position = tc.position;
            p2.velocity = pc.velocity + noise;
            p2.acceleration = pc.acceleration;
            p2.life = pc.life;
            p2.scale = pc.scale;
            emit_particle(ps, p2);

            pc.accumulator -= pc.interval;
        }
    }
}

}