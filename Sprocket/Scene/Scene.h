#pragma once
#include "ECS.h"
#include "EntitySystem.h"
#include "Event.h"
#include "Maths.h"
#include "ECS.h"

#include <memory>
#include <vector>
#include <functional>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

#include <tsl/hopscotch_map.h>

namespace Sprocket {

struct Sun
{
    glm::vec3 direction;
    glm::vec3 colour;
    float     brightness;
};

struct Ambience
{
    glm::vec3 colour;
    float     brightness;
};
    
class Scene
{
public:
    using EntityCallback = std::function<void(ECS::Entity&)>;

    using TypeFunctionMap = tsl::hopscotch_map<
        std::type_index, std::vector<EntityCallback>
    >;

private:
    std::vector<std::shared_ptr<EntitySystem>> d_systems;

    ECS::Registry d_registry;

    Sun d_sun;
    Ambience d_ambience;

public:
    Scene();
    ~Scene();

    ECS::Registry* Reg() { return &d_registry; }

    void AddSystem(std::shared_ptr<EntitySystem> system);
    void ClearSystems();

    void OnStartup();
    void OnUpdate(double dt);
    void OnEvent(Event& event);

    std::size_t Size() const;

    template <typename... Components> void Each(EntityCallback func);
        // Loops through all the entities that have the specified
        // components and calls the given lambda with each entity
        // as the argument.

    void Clear();

    Sun& GetSun() { return d_sun; }
    const Sun& GetSun() const { return d_sun; }

    Ambience& GetAmbience() { return d_ambience; }
    const Ambience& GetAmbience() const { return d_ambience; }
};

template <typename... Components>
void Scene::Each(EntityCallback lambda)
{
    for (ECS::Entity e : d_registry.View<Components...>()) {
        lambda(e);   
    }
}

}
