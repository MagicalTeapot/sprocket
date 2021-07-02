#include "Loader.h"
#include "Log.h"
#include "Components.h"
#include "Maths.h"
#include "Yaml.h"
#include "Scene.h"

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <memory>

namespace spkt {
namespace Loader {

void Save(const std::string& file, spkt::registry* reg)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Entities" << YAML::BeginSeq;
    for (auto id : reg->all()) {
        spkt::entity entity{*reg, id};

        // Don't save runtime entities
        if (entity.has<Runtime>()) { continue; }

        out << YAML::BeginMap;
        out << YAML::Key << "ID#" << YAML::Value << id;
        if (entity.has<NameComponent>()) {
            const auto& c = entity.get<NameComponent>();
            out << YAML::Key << "NameComponent" << YAML::BeginMap;
            out << YAML::Key << "name" << YAML::Value << c.name;
            out << YAML::EndMap;
        }
        if (entity.has<Transform2DComponent>()) {
            const auto& c = entity.get<Transform2DComponent>();
            out << YAML::Key << "Transform2DComponent" << YAML::BeginMap;
            out << YAML::Key << "position" << YAML::Value << c.position;
            out << YAML::Key << "rotation" << YAML::Value << c.rotation;
            out << YAML::Key << "scale" << YAML::Value << c.scale;
            out << YAML::EndMap;
        }
        if (entity.has<Transform3DComponent>()) {
            const auto& c = entity.get<Transform3DComponent>();
            out << YAML::Key << "Transform3DComponent" << YAML::BeginMap;
            out << YAML::Key << "position" << YAML::Value << c.position;
            out << YAML::Key << "orientation" << YAML::Value << c.orientation;
            out << YAML::Key << "scale" << YAML::Value << c.scale;
            out << YAML::EndMap;
        }
        if (entity.has<ModelComponent>()) {
            const auto& c = entity.get<ModelComponent>();
            out << YAML::Key << "ModelComponent" << YAML::BeginMap;
            out << YAML::Key << "mesh" << YAML::Value << c.mesh;
            out << YAML::Key << "material" << YAML::Value << c.material;
            out << YAML::EndMap;
        }
        if (entity.has<RigidBody3DComponent>()) {
            const auto& c = entity.get<RigidBody3DComponent>();
            out << YAML::Key << "RigidBody3DComponent" << YAML::BeginMap;
            out << YAML::Key << "velocity" << YAML::Value << c.velocity;
            out << YAML::Key << "gravity" << YAML::Value << c.gravity;
            out << YAML::Key << "frozen" << YAML::Value << c.frozen;
            out << YAML::Key << "bounciness" << YAML::Value << c.bounciness;
            out << YAML::Key << "frictionCoefficient" << YAML::Value << c.frictionCoefficient;
            out << YAML::Key << "rollingResistance" << YAML::Value << c.rollingResistance;
            out << YAML::EndMap;
        }
        if (entity.has<BoxCollider3DComponent>()) {
            const auto& c = entity.get<BoxCollider3DComponent>();
            out << YAML::Key << "BoxCollider3DComponent" << YAML::BeginMap;
            out << YAML::Key << "position" << YAML::Value << c.position;
            out << YAML::Key << "orientation" << YAML::Value << c.orientation;
            out << YAML::Key << "mass" << YAML::Value << c.mass;
            out << YAML::Key << "halfExtents" << YAML::Value << c.halfExtents;
            out << YAML::Key << "applyScale" << YAML::Value << c.applyScale;
            out << YAML::EndMap;
        }
        if (entity.has<SphereCollider3DComponent>()) {
            const auto& c = entity.get<SphereCollider3DComponent>();
            out << YAML::Key << "SphereCollider3DComponent" << YAML::BeginMap;
            out << YAML::Key << "position" << YAML::Value << c.position;
            out << YAML::Key << "orientation" << YAML::Value << c.orientation;
            out << YAML::Key << "mass" << YAML::Value << c.mass;
            out << YAML::Key << "radius" << YAML::Value << c.radius;
            out << YAML::EndMap;
        }
        if (entity.has<CapsuleCollider3DComponent>()) {
            const auto& c = entity.get<CapsuleCollider3DComponent>();
            out << YAML::Key << "CapsuleCollider3DComponent" << YAML::BeginMap;
            out << YAML::Key << "position" << YAML::Value << c.position;
            out << YAML::Key << "orientation" << YAML::Value << c.orientation;
            out << YAML::Key << "mass" << YAML::Value << c.mass;
            out << YAML::Key << "radius" << YAML::Value << c.radius;
            out << YAML::Key << "height" << YAML::Value << c.height;
            out << YAML::EndMap;
        }
        if (entity.has<ScriptComponent>()) {
            const auto& c = entity.get<ScriptComponent>();
            out << YAML::Key << "ScriptComponent" << YAML::BeginMap;
            out << YAML::Key << "script" << YAML::Value << c.script;
            out << YAML::Key << "active" << YAML::Value << c.active;
            out << YAML::EndMap;
        }
        if (entity.has<Camera3DComponent>()) {
            const auto& c = entity.get<Camera3DComponent>();
            out << YAML::Key << "Camera3DComponent" << YAML::BeginMap;
            out << YAML::Key << "fov" << YAML::Value << c.fov;
            out << YAML::Key << "pitch" << YAML::Value << c.pitch;
            out << YAML::EndMap;
        }
        if (entity.has<PathComponent>()) {
            const auto& c = entity.get<PathComponent>();
            out << YAML::Key << "PathComponent" << YAML::BeginMap;
            out << YAML::Key << "speed" << YAML::Value << c.speed;
            out << YAML::EndMap;
        }
        if (entity.has<LightComponent>()) {
            const auto& c = entity.get<LightComponent>();
            out << YAML::Key << "LightComponent" << YAML::BeginMap;
            out << YAML::Key << "colour" << YAML::Value << c.colour;
            out << YAML::Key << "brightness" << YAML::Value << c.brightness;
            out << YAML::EndMap;
        }
        if (entity.has<SunComponent>()) {
            const auto& c = entity.get<SunComponent>();
            out << YAML::Key << "SunComponent" << YAML::BeginMap;
            out << YAML::Key << "colour" << YAML::Value << c.colour;
            out << YAML::Key << "brightness" << YAML::Value << c.brightness;
            out << YAML::Key << "direction" << YAML::Value << c.direction;
            out << YAML::Key << "shadows" << YAML::Value << c.shadows;
            out << YAML::EndMap;
        }
        if (entity.has<AmbienceComponent>()) {
            const auto& c = entity.get<AmbienceComponent>();
            out << YAML::Key << "AmbienceComponent" << YAML::BeginMap;
            out << YAML::Key << "colour" << YAML::Value << c.colour;
            out << YAML::Key << "brightness" << YAML::Value << c.brightness;
            out << YAML::EndMap;
        }
        if (entity.has<ParticleComponent>()) {
            const auto& c = entity.get<ParticleComponent>();
            out << YAML::Key << "ParticleComponent" << YAML::BeginMap;
            out << YAML::Key << "interval" << YAML::Value << c.interval;
            out << YAML::Key << "velocity" << YAML::Value << c.velocity;
            out << YAML::Key << "velocityNoise" << YAML::Value << c.velocityNoise;
            out << YAML::Key << "acceleration" << YAML::Value << c.acceleration;
            out << YAML::Key << "scale" << YAML::Value << c.scale;
            out << YAML::Key << "life" << YAML::Value << c.life;
            out << YAML::EndMap;
        }
        if (entity.has<MeshAnimationComponent>()) {
            const auto& c = entity.get<MeshAnimationComponent>();
            out << YAML::Key << "MeshAnimationComponent" << YAML::BeginMap;
            out << YAML::Key << "name" << YAML::Value << c.name;
            out << YAML::Key << "time" << YAML::Value << c.time;
            out << YAML::Key << "speed" << YAML::Value << c.speed;
            out << YAML::EndMap;
        }
        if (entity.has<TileMapSingleton>()) {
            const auto& c = entity.get<TileMapSingleton>();
            out << YAML::Key << "TileMapSingleton" << YAML::BeginMap;
            out << YAML::Key << "tiles" << YAML::Value << c.tiles;
            out << YAML::EndMap;
        }
        out << YAML::EndMap;
    }
    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream fout(file);
    fout << out.c_str();
}

void Load(const std::string& file, spkt::registry* reg)
{
    std::ifstream stream(file);
    std::stringstream sstream;
    sstream << stream.rdbuf();

    YAML::Node data = YAML::Load(sstream.str());

    if (!data["Entities"]) {
        return; // TODO: Error checking
    }

    auto entities = data["Entities"];
    std::unordered_map<apx::entity, apx::entity> id_remapper;

    // Performs any extra transformations to values that cannot be done during
    // yaml decoding, for example converting entity IDs to their new values.
    const auto transform = [&](auto&& param) {
        if constexpr (std::is_same_v<decltype(param), apx::entity>) {
            return id_remapper[param];
        }
        return param;
    };

    for (auto entity : entities) {
        apx::entity old_id = entity["ID#"].as<apx::entity>();
        apx::entity new_id = reg->create();
        id_remapper[old_id] = new_id;
    }

    for (auto entity : entities) {
        apx::entity old_id = entity["ID#"].as<apx::entity>();
        spkt::entity e{*reg, id_remapper[old_id]};
        if (auto spec = entity["NameComponent"]) {
            NameComponent c;
            c.name = transform(spec["name"].as<std::string>());
            e.add<NameComponent>(c);
        }
        if (auto spec = entity["Transform2DComponent"]) {
            Transform2DComponent c;
            c.position = transform(spec["position"].as<glm::vec2>());
            c.rotation = transform(spec["rotation"].as<float>());
            c.scale = transform(spec["scale"].as<glm::vec2>());
            e.add<Transform2DComponent>(c);
        }
        if (auto spec = entity["Transform3DComponent"]) {
            Transform3DComponent c;
            c.position = transform(spec["position"].as<glm::vec3>());
            c.orientation = transform(spec["orientation"].as<glm::quat>());
            c.scale = transform(spec["scale"].as<glm::vec3>());
            e.add<Transform3DComponent>(c);
        }
        if (auto spec = entity["ModelComponent"]) {
            ModelComponent c;
            c.mesh = transform(spec["mesh"].as<std::string>());
            c.material = transform(spec["material"].as<std::string>());
            e.add<ModelComponent>(c);
        }
        if (auto spec = entity["RigidBody3DComponent"]) {
            RigidBody3DComponent c;
            c.velocity = transform(spec["velocity"].as<glm::vec3>());
            c.gravity = transform(spec["gravity"].as<bool>());
            c.frozen = transform(spec["frozen"].as<bool>());
            c.bounciness = transform(spec["bounciness"].as<float>());
            c.frictionCoefficient = transform(spec["frictionCoefficient"].as<float>());
            c.rollingResistance = transform(spec["rollingResistance"].as<float>());
            e.add<RigidBody3DComponent>(c);
        }
        if (auto spec = entity["BoxCollider3DComponent"]) {
            BoxCollider3DComponent c;
            c.position = transform(spec["position"].as<glm::vec3>());
            c.orientation = transform(spec["orientation"].as<glm::quat>());
            c.mass = transform(spec["mass"].as<float>());
            c.halfExtents = transform(spec["halfExtents"].as<glm::vec3>());
            c.applyScale = transform(spec["applyScale"].as<bool>());
            e.add<BoxCollider3DComponent>(c);
        }
        if (auto spec = entity["SphereCollider3DComponent"]) {
            SphereCollider3DComponent c;
            c.position = transform(spec["position"].as<glm::vec3>());
            c.orientation = transform(spec["orientation"].as<glm::quat>());
            c.mass = transform(spec["mass"].as<float>());
            c.radius = transform(spec["radius"].as<float>());
            e.add<SphereCollider3DComponent>(c);
        }
        if (auto spec = entity["CapsuleCollider3DComponent"]) {
            CapsuleCollider3DComponent c;
            c.position = transform(spec["position"].as<glm::vec3>());
            c.orientation = transform(spec["orientation"].as<glm::quat>());
            c.mass = transform(spec["mass"].as<float>());
            c.radius = transform(spec["radius"].as<float>());
            c.height = transform(spec["height"].as<float>());
            e.add<CapsuleCollider3DComponent>(c);
        }
        if (auto spec = entity["ScriptComponent"]) {
            ScriptComponent c;
            c.script = transform(spec["script"].as<std::string>());
            c.active = transform(spec["active"].as<bool>());
            e.add<ScriptComponent>(c);
        }
        if (auto spec = entity["Camera3DComponent"]) {
            Camera3DComponent c;
            c.fov = transform(spec["fov"].as<float>());
            c.pitch = transform(spec["pitch"].as<float>());
            e.add<Camera3DComponent>(c);
        }
        if (auto spec = entity["PathComponent"]) {
            PathComponent c;
            c.speed = transform(spec["speed"].as<float>());
            e.add<PathComponent>(c);
        }
        if (auto spec = entity["LightComponent"]) {
            LightComponent c;
            c.colour = transform(spec["colour"].as<glm::vec3>());
            c.brightness = transform(spec["brightness"].as<float>());
            e.add<LightComponent>(c);
        }
        if (auto spec = entity["SunComponent"]) {
            SunComponent c;
            c.colour = transform(spec["colour"].as<glm::vec3>());
            c.brightness = transform(spec["brightness"].as<float>());
            c.direction = transform(spec["direction"].as<glm::vec3>());
            c.shadows = transform(spec["shadows"].as<bool>());
            e.add<SunComponent>(c);
        }
        if (auto spec = entity["AmbienceComponent"]) {
            AmbienceComponent c;
            c.colour = transform(spec["colour"].as<glm::vec3>());
            c.brightness = transform(spec["brightness"].as<float>());
            e.add<AmbienceComponent>(c);
        }
        if (auto spec = entity["ParticleComponent"]) {
            ParticleComponent c;
            c.interval = transform(spec["interval"].as<float>());
            c.velocity = transform(spec["velocity"].as<glm::vec3>());
            c.velocityNoise = transform(spec["velocityNoise"].as<float>());
            c.acceleration = transform(spec["acceleration"].as<glm::vec3>());
            c.scale = transform(spec["scale"].as<glm::vec3>());
            c.life = transform(spec["life"].as<float>());
            e.add<ParticleComponent>(c);
        }
        if (auto spec = entity["MeshAnimationComponent"]) {
            MeshAnimationComponent c;
            c.name = transform(spec["name"].as<std::string>());
            c.time = transform(spec["time"].as<float>());
            c.speed = transform(spec["speed"].as<float>());
            e.add<MeshAnimationComponent>(c);
        }
        if (auto spec = entity["TileMapSingleton"]) {
            TileMapSingleton c;
            c.tiles = transform(spec["tiles"].as<std::unordered_map<glm::ivec2, apx::entity>>());
            e.add<TileMapSingleton>(c);
        }
    }
}

apx::entity Copy(spkt::registry* reg, apx::entity entity)
{
    apx::entity e = reg->create();
    if (reg->has<Runtime>(entity)) {
        reg->add<Runtime>(e, reg->get<Runtime>(entity));
    }
    if (reg->has<Singleton>(entity)) {
        reg->add<Singleton>(e, reg->get<Singleton>(entity));
    }
    if (reg->has<Event>(entity)) {
        reg->add<Event>(e, reg->get<Event>(entity));
    }
    if (reg->has<NameComponent>(entity)) {
        reg->add<NameComponent>(e, reg->get<NameComponent>(entity));
    }
    if (reg->has<Transform2DComponent>(entity)) {
        reg->add<Transform2DComponent>(e, reg->get<Transform2DComponent>(entity));
    }
    if (reg->has<Transform3DComponent>(entity)) {
        reg->add<Transform3DComponent>(e, reg->get<Transform3DComponent>(entity));
    }
    if (reg->has<ModelComponent>(entity)) {
        reg->add<ModelComponent>(e, reg->get<ModelComponent>(entity));
    }
    if (reg->has<RigidBody3DComponent>(entity)) {
        reg->add<RigidBody3DComponent>(e, reg->get<RigidBody3DComponent>(entity));
    }
    if (reg->has<BoxCollider3DComponent>(entity)) {
        reg->add<BoxCollider3DComponent>(e, reg->get<BoxCollider3DComponent>(entity));
    }
    if (reg->has<SphereCollider3DComponent>(entity)) {
        reg->add<SphereCollider3DComponent>(e, reg->get<SphereCollider3DComponent>(entity));
    }
    if (reg->has<CapsuleCollider3DComponent>(entity)) {
        reg->add<CapsuleCollider3DComponent>(e, reg->get<CapsuleCollider3DComponent>(entity));
    }
    if (reg->has<ScriptComponent>(entity)) {
        reg->add<ScriptComponent>(e, reg->get<ScriptComponent>(entity));
    }
    if (reg->has<Camera3DComponent>(entity)) {
        reg->add<Camera3DComponent>(e, reg->get<Camera3DComponent>(entity));
    }
    if (reg->has<PathComponent>(entity)) {
        reg->add<PathComponent>(e, reg->get<PathComponent>(entity));
    }
    if (reg->has<LightComponent>(entity)) {
        reg->add<LightComponent>(e, reg->get<LightComponent>(entity));
    }
    if (reg->has<SunComponent>(entity)) {
        reg->add<SunComponent>(e, reg->get<SunComponent>(entity));
    }
    if (reg->has<AmbienceComponent>(entity)) {
        reg->add<AmbienceComponent>(e, reg->get<AmbienceComponent>(entity));
    }
    if (reg->has<ParticleComponent>(entity)) {
        reg->add<ParticleComponent>(e, reg->get<ParticleComponent>(entity));
    }
    if (reg->has<MeshAnimationComponent>(entity)) {
        reg->add<MeshAnimationComponent>(e, reg->get<MeshAnimationComponent>(entity));
    }
    if (reg->has<CollisionEvent>(entity)) {
        reg->add<CollisionEvent>(e, reg->get<CollisionEvent>(entity));
    }
    if (reg->has<PhysicsSingleton>(entity)) {
        reg->add<PhysicsSingleton>(e, reg->get<PhysicsSingleton>(entity));
    }
    if (reg->has<InputSingleton>(entity)) {
        reg->add<InputSingleton>(e, reg->get<InputSingleton>(entity));
    }
    if (reg->has<GameGridSingleton>(entity)) {
        reg->add<GameGridSingleton>(e, reg->get<GameGridSingleton>(entity));
    }
    if (reg->has<TileMapSingleton>(entity)) {
        reg->add<TileMapSingleton>(e, reg->get<TileMapSingleton>(entity));
    }
    if (reg->has<CameraSingleton>(entity)) {
        reg->add<CameraSingleton>(e, reg->get<CameraSingleton>(entity));
    }
    if (reg->has<ParticleSingleton>(entity)) {
        reg->add<ParticleSingleton>(e, reg->get<ParticleSingleton>(entity));
    }
    return e;
}

void Copy(spkt::registry* source, spkt::registry* target)
{
    // First, set up new handles in the target scene and create a mapping between
    // new and old IDs.
    std::unordered_map<apx::entity, apx::entity> id_remapper;
    for (auto id : source->all()) {
        apx::entity new_id = target->create();
        id_remapper[id] = new_id;
    }

    const auto transform = [&] <typename T> (T&& param) {
        if constexpr (std::is_same_v<decltype(param), apx::entity>) {
            return id_remapper[param];
        }
        return std::forward<T>(param);
    };

    for (auto id : source->all()) {
        spkt::entity src{*source, id};
        spkt::entity dst{*target, id_remapper[id]};
        if (src.has<Runtime>()) {
            const Runtime& source_comp = src.get<Runtime>();
            Runtime target_comp;
            dst.add<Runtime>(target_comp);
        }
        if (src.has<Singleton>()) {
            const Singleton& source_comp = src.get<Singleton>();
            Singleton target_comp;
            dst.add<Singleton>(target_comp);
        }
        if (src.has<Event>()) {
            const Event& source_comp = src.get<Event>();
            Event target_comp;
            dst.add<Event>(target_comp);
        }
        if (src.has<NameComponent>()) {
            const NameComponent& source_comp = src.get<NameComponent>();
            NameComponent target_comp;
            target_comp.name = transform(source_comp.name);
            dst.add<NameComponent>(target_comp);
        }
        if (src.has<Transform2DComponent>()) {
            const Transform2DComponent& source_comp = src.get<Transform2DComponent>();
            Transform2DComponent target_comp;
            target_comp.position = transform(source_comp.position);
            target_comp.rotation = transform(source_comp.rotation);
            target_comp.scale = transform(source_comp.scale);
            dst.add<Transform2DComponent>(target_comp);
        }
        if (src.has<Transform3DComponent>()) {
            const Transform3DComponent& source_comp = src.get<Transform3DComponent>();
            Transform3DComponent target_comp;
            target_comp.position = transform(source_comp.position);
            target_comp.orientation = transform(source_comp.orientation);
            target_comp.scale = transform(source_comp.scale);
            dst.add<Transform3DComponent>(target_comp);
        }
        if (src.has<ModelComponent>()) {
            const ModelComponent& source_comp = src.get<ModelComponent>();
            ModelComponent target_comp;
            target_comp.mesh = transform(source_comp.mesh);
            target_comp.material = transform(source_comp.material);
            dst.add<ModelComponent>(target_comp);
        }
        if (src.has<RigidBody3DComponent>()) {
            const RigidBody3DComponent& source_comp = src.get<RigidBody3DComponent>();
            RigidBody3DComponent target_comp;
            target_comp.velocity = transform(source_comp.velocity);
            target_comp.gravity = transform(source_comp.gravity);
            target_comp.frozen = transform(source_comp.frozen);
            target_comp.bounciness = transform(source_comp.bounciness);
            target_comp.frictionCoefficient = transform(source_comp.frictionCoefficient);
            target_comp.rollingResistance = transform(source_comp.rollingResistance);
            target_comp.force = transform(source_comp.force);
            target_comp.onFloor = transform(source_comp.onFloor);
            target_comp.runtime = transform(source_comp.runtime);
            dst.add<RigidBody3DComponent>(target_comp);
        }
        if (src.has<BoxCollider3DComponent>()) {
            const BoxCollider3DComponent& source_comp = src.get<BoxCollider3DComponent>();
            BoxCollider3DComponent target_comp;
            target_comp.position = transform(source_comp.position);
            target_comp.orientation = transform(source_comp.orientation);
            target_comp.mass = transform(source_comp.mass);
            target_comp.halfExtents = transform(source_comp.halfExtents);
            target_comp.applyScale = transform(source_comp.applyScale);
            target_comp.runtime = transform(source_comp.runtime);
            dst.add<BoxCollider3DComponent>(target_comp);
        }
        if (src.has<SphereCollider3DComponent>()) {
            const SphereCollider3DComponent& source_comp = src.get<SphereCollider3DComponent>();
            SphereCollider3DComponent target_comp;
            target_comp.position = transform(source_comp.position);
            target_comp.orientation = transform(source_comp.orientation);
            target_comp.mass = transform(source_comp.mass);
            target_comp.radius = transform(source_comp.radius);
            target_comp.runtime = transform(source_comp.runtime);
            dst.add<SphereCollider3DComponent>(target_comp);
        }
        if (src.has<CapsuleCollider3DComponent>()) {
            const CapsuleCollider3DComponent& source_comp = src.get<CapsuleCollider3DComponent>();
            CapsuleCollider3DComponent target_comp;
            target_comp.position = transform(source_comp.position);
            target_comp.orientation = transform(source_comp.orientation);
            target_comp.mass = transform(source_comp.mass);
            target_comp.radius = transform(source_comp.radius);
            target_comp.height = transform(source_comp.height);
            target_comp.runtime = transform(source_comp.runtime);
            dst.add<CapsuleCollider3DComponent>(target_comp);
        }
        if (src.has<ScriptComponent>()) {
            const ScriptComponent& source_comp = src.get<ScriptComponent>();
            ScriptComponent target_comp;
            target_comp.script = transform(source_comp.script);
            target_comp.active = transform(source_comp.active);
            target_comp.script_runtime = transform(source_comp.script_runtime);
            dst.add<ScriptComponent>(target_comp);
        }
        if (src.has<Camera3DComponent>()) {
            const Camera3DComponent& source_comp = src.get<Camera3DComponent>();
            Camera3DComponent target_comp;
            target_comp.projection = transform(source_comp.projection);
            target_comp.fov = transform(source_comp.fov);
            target_comp.pitch = transform(source_comp.pitch);
            dst.add<Camera3DComponent>(target_comp);
        }
        if (src.has<PathComponent>()) {
            const PathComponent& source_comp = src.get<PathComponent>();
            PathComponent target_comp;
            target_comp.markers = transform(source_comp.markers);
            target_comp.speed = transform(source_comp.speed);
            dst.add<PathComponent>(target_comp);
        }
        if (src.has<LightComponent>()) {
            const LightComponent& source_comp = src.get<LightComponent>();
            LightComponent target_comp;
            target_comp.colour = transform(source_comp.colour);
            target_comp.brightness = transform(source_comp.brightness);
            dst.add<LightComponent>(target_comp);
        }
        if (src.has<SunComponent>()) {
            const SunComponent& source_comp = src.get<SunComponent>();
            SunComponent target_comp;
            target_comp.colour = transform(source_comp.colour);
            target_comp.brightness = transform(source_comp.brightness);
            target_comp.direction = transform(source_comp.direction);
            target_comp.shadows = transform(source_comp.shadows);
            dst.add<SunComponent>(target_comp);
        }
        if (src.has<AmbienceComponent>()) {
            const AmbienceComponent& source_comp = src.get<AmbienceComponent>();
            AmbienceComponent target_comp;
            target_comp.colour = transform(source_comp.colour);
            target_comp.brightness = transform(source_comp.brightness);
            dst.add<AmbienceComponent>(target_comp);
        }
        if (src.has<ParticleComponent>()) {
            const ParticleComponent& source_comp = src.get<ParticleComponent>();
            ParticleComponent target_comp;
            target_comp.interval = transform(source_comp.interval);
            target_comp.velocity = transform(source_comp.velocity);
            target_comp.velocityNoise = transform(source_comp.velocityNoise);
            target_comp.acceleration = transform(source_comp.acceleration);
            target_comp.scale = transform(source_comp.scale);
            target_comp.life = transform(source_comp.life);
            target_comp.accumulator = transform(source_comp.accumulator);
            dst.add<ParticleComponent>(target_comp);
        }
        if (src.has<MeshAnimationComponent>()) {
            const MeshAnimationComponent& source_comp = src.get<MeshAnimationComponent>();
            MeshAnimationComponent target_comp;
            target_comp.name = transform(source_comp.name);
            target_comp.time = transform(source_comp.time);
            target_comp.speed = transform(source_comp.speed);
            dst.add<MeshAnimationComponent>(target_comp);
        }
        if (src.has<CollisionEvent>()) {
            const CollisionEvent& source_comp = src.get<CollisionEvent>();
            CollisionEvent target_comp;
            target_comp.entity_a = transform(source_comp.entity_a);
            target_comp.entity_b = transform(source_comp.entity_b);
            dst.add<CollisionEvent>(target_comp);
        }
        if (src.has<PhysicsSingleton>()) {
            const PhysicsSingleton& source_comp = src.get<PhysicsSingleton>();
            PhysicsSingleton target_comp;
            target_comp.physics_runtime = transform(source_comp.physics_runtime);
            dst.add<PhysicsSingleton>(target_comp);
        }
        if (src.has<InputSingleton>()) {
            const InputSingleton& source_comp = src.get<InputSingleton>();
            InputSingleton target_comp;
            target_comp.keyboard = transform(source_comp.keyboard);
            target_comp.mouse = transform(source_comp.mouse);
            target_comp.mouse_click = transform(source_comp.mouse_click);
            target_comp.mouse_unclick = transform(source_comp.mouse_unclick);
            target_comp.mouse_pos = transform(source_comp.mouse_pos);
            target_comp.mouse_offset = transform(source_comp.mouse_offset);
            target_comp.mouse_scrolled = transform(source_comp.mouse_scrolled);
            target_comp.window_width = transform(source_comp.window_width);
            target_comp.window_height = transform(source_comp.window_height);
            target_comp.window_resized = transform(source_comp.window_resized);
            dst.add<InputSingleton>(target_comp);
        }
        if (src.has<GameGridSingleton>()) {
            const GameGridSingleton& source_comp = src.get<GameGridSingleton>();
            GameGridSingleton target_comp;
            target_comp.hovered_square_entity = transform(source_comp.hovered_square_entity);
            target_comp.clicked_square_entity = transform(source_comp.clicked_square_entity);
            target_comp.hovered_square = transform(source_comp.hovered_square);
            target_comp.clicked_square = transform(source_comp.clicked_square);
            dst.add<GameGridSingleton>(target_comp);
        }
        if (src.has<TileMapSingleton>()) {
            const TileMapSingleton& source_comp = src.get<TileMapSingleton>();
            TileMapSingleton target_comp;
            target_comp.tiles = transform(source_comp.tiles);
            dst.add<TileMapSingleton>(target_comp);
        }
        if (src.has<CameraSingleton>()) {
            const CameraSingleton& source_comp = src.get<CameraSingleton>();
            CameraSingleton target_comp;
            target_comp.camera_entity = transform(source_comp.camera_entity);
            dst.add<CameraSingleton>(target_comp);
        }
        if (src.has<ParticleSingleton>()) {
            const ParticleSingleton& source_comp = src.get<ParticleSingleton>();
            ParticleSingleton target_comp;
            target_comp.particle_manager = transform(source_comp.particle_manager);
            dst.add<ParticleSingleton>(target_comp);
        }
    }
}

}
}
