#pragma once
#include "ECS.h"
#include "EntitySystem.h"
#include "Events.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <string_view>

namespace Sprocket {
    
class Scene
{
    std::unordered_map<std::size_t, std::size_t> d_lookup;
    std::vector<std::unique_ptr<EntitySystem>> d_systems;

    ecs::Registry d_registry;

public:
    Scene();

    ecs::Registry& Entities() { return d_registry; }

    template <typename T, typename... Args>
    T& Add(Args&&... args);

    template <typename T> bool Has();
    template <typename T> T& Get();

    void Load(std::string_view file);

    void OnUpdate(double dt);
    void OnEvent(ev::Event& event);

    std::size_t Size() const;

    void Clear();
};

template <typename T, typename... Args>
T& Scene::Add(Args&&... args)
{
    assert(d_registry.size() == 0);
    assert(d_lookup.find(spkt::type_hash<T>) == d_lookup.end());
    d_lookup[spkt::type_hash<T>] = d_systems.size();
    d_systems.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
    d_systems.back()->OnStartup(d_registry);
    return *static_cast<T*>(d_systems.back().get());
}

template <typename T> bool Scene::Has()
{
    return d_lookup.find(type_hash<T>) != d_lookup.end();
}

template <typename T> T& Scene::Get()
{
    auto it = d_lookup.find(spkt::type_hash<T>);
    assert(it != d_lookup.end());
    return *static_cast<T*>(d_systems[it->second].get());
}

}
