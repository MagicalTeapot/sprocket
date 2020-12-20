#pragma once
#include "Types.h"
#include "SparseSet.h"
#include "Log.h"

#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <typeinfo>
#include <array>
#include <memory>
#include <queue>
#include <functional>
#include <deque>
#include <limits>
#include <vector>
#include <cassert>
#include <any>

#include <cppcoro/generator.hpp>

namespace Sprocket {
namespace ECS {
    
class Registry;

static constexpr u32 NULL_ID = std::numeric_limits<u32>::max();

class Entity
{
    ECS::Registry* d_registry;
    u32            d_id;

    void Remove(std::type_index type);

public:
    Entity(ECS::Registry* r, u32 i) : d_registry(r), d_id(i) {}
    Entity() : d_registry(nullptr), d_id(NULL_ID) {}

    bool Valid() const;
    void Delete();

    template <typename Comp> Comp& Add();
    template <typename Comp> Comp& Add(const Comp& component);
    template <typename Comp, typename... Args> Comp& Emplace(Args&&... args);
    template <typename Comp> void Remove();
    template <typename Comp> Comp& Get();
    template <typename Comp> const Comp& Get() const;
    template <typename Comp> bool Has() const;

    u32 Id() const;
    u16 Slot() const;
    u16 Version() const;

    bool operator==(Entity other) const;
    bool operator!=(Entity other) const;
    Entity& operator=(Entity other);

    // TODO: Remove
    Entity NewEntity() const;
};

class Registry
{
public:
    using EntityCallback = std::function<void(Entity)>;
    using EntityPredicate = std::function<bool(Entity)>;

private:
    // Stores the current version of each entity.
    SparseSet<u16> d_entities;

    // When an entity is removed, their slot/version is added to the pool so that it
    // can be reused.
    std::queue<std::pair<u16, u16>> d_pool;

    // Store of all components for all entities. The type of the components are erased.
    struct ComponentData
    {
        // All instances of this component.
        SparseSet<std::any> instances;

        // Callbacks triggered whenever a new instance of this component is added. These
        // are called after the component has been added.
        std::vector<EntityCallback> onAdd;

        // Callbacks triggers whenever an instance of this component is remove. These
        // are called before the component has been removed.
        std::vector<EntityCallback> onRemove;
    };

    std::unordered_map<std::type_index, ComponentData> d_comps;

public:
    Registry() = default;

    Entity New();

    // Loops through all entities and deletes their components. This will trigger
    // the OnRemove functionality.
    void Clear();

    // Number of active entities in the registry.
    std::size_t Size() const;

    // Returns the first entity satisfying the given predicate, or ECS::Null if
    // none is found.
    Entity Find(const EntityPredicate& pred);

    // Generates all active entities. This is fast, however adding and removing
    // entities while iterating results is undefined.
    cppcoro::generator<Entity> Fast();

    // Generates all active entities. This is slow, but adding and removing
    // entities is safe. If an entity is added, it may or may not be included
    // in the iteration, depending on where in the sparse set it gets added.
    cppcoro::generator<Entity> Safe();

    // Does a fast iteration over all entities with the given Comp. If any extra
    // component types are specified, only entities that have all of those types
    // will be yielded. This should only be used for modifying the components, not
    // adding/removing new ones.
    template <typename Comp, typename... Rest> cppcoro::generator<Entity> View();

    template <typename Comp> void OnAdd(const EntityCallback& cb);
    template <typename Comp> void OnRemove(const EntityCallback& cb);

    static u32 GetID(u16 slot, u16 version);

    friend class Entity;
};

// An "empty" entity.
static const Entity Null(nullptr, NULL_ID);

// ==============================================================
//                      TEMPLATE DEFINITIONS
// ==============================================================

// REGISTRY TEMPLATES

template <typename Comp>
void Registry::OnAdd(const EntityCallback& cb)
{
    d_comps[typeid(Comp)].onAdd.push_back(cb);
}

template <typename Comp>
void Registry::OnRemove(const EntityCallback& cb)
{
    d_comps[typeid(Comp)].onRemove.push_back(cb);
}

template <typename Comp, typename... Rest>
cppcoro::generator<Entity> Registry::View()
{
    for (auto& [index, comp] : d_comps[typeid(Comp)].instances.Fast()) {
        Entity entity{this, GetID(index, d_entities[index])};
        if ((entity.Has<Rest>() && ...)) {
            co_yield entity;
        }
    }
}

// ENTITY TEMPLATES

template <typename Comp>
Comp& Entity::Add()
{
    return Add<Comp>(Comp{});
}

template <typename Comp>
Comp& Entity::Add(const Comp& component)
{
    assert(Valid());
    auto& entry = d_registry->d_comps[typeid(Comp)].instances[Slot()];
    entry = component;

    for (const auto& cb : d_registry->d_comps[typeid(Comp)].onAdd) {
        cb(*this);
    }

    return std::any_cast<Comp&>(entry);
}

template <typename Comp, typename... Args>
Comp& Entity::Emplace(Args&&... args)
{
    assert(Valid());
    auto& entry = d_registry->d_comps[typeid(Comp)].instances.Insert(
        Slot(), std::make_any<Comp&>(std::forward<Args>(args)...)
    );

    for (const auto& cb : d_registry->d_comps[typeid(Comp)].onAdd) {
        cb(*this);
    }

    return std::any_cast<Comp&>(entry);
}

template <typename Comp>
void Entity::Remove()
{
    assert(Valid());
    Remove(typeid(Comp));
}

template <typename Comp>
Comp& Entity::Get()
{
    assert(Valid());
    auto& entry = d_registry->d_comps.at(typeid(Comp)).instances[Slot()];
    return std::any_cast<Comp&>(entry);
}

template <typename Comp>
const Comp& Entity::Get() const
{
    assert(Valid());
    auto& entry = d_registry->d_comps.at(typeid(Comp)).instances[Slot()];
    return std::any_cast<Comp&>(entry);
}

template <typename Comp>
bool Entity::Has() const
{
    assert(Valid());
    if (auto it = d_registry->d_comps.find(typeid(Comp)); it != d_registry->d_comps.end()) {
        if (it->second.instances.Has(Slot())) {
            const auto& entry = it->second.instances[Slot()];
            return entry.has_value();
        }
    }
    return false;
}

}
}