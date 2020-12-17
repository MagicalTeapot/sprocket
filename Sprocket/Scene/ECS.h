#pragma once
#include "Types.h"

#include <unordered_map>
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

#include "Log.h"

namespace Sprocket {
namespace ECS {
    
class Registry;

static constexpr u32 NULL_ID = std::numeric_limits<u32>::max();

class Entity
{
    ECS::Registry* d_registry;
    u32            d_id;

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

private:

    static constexpr std::size_t NUM_ENTITIES = std::numeric_limits<u16>::max();

    // When an entity is removed, their slot is added to the pool so that it can be reused.
    std::queue<u16> d_pool;

    // If the pool of slots is empty, then the next entity will use this variable as their slot.
    u16 d_next;

    // We also keep track of the number of times a slot has been used for validity checks.
    std::array<u16, NUM_ENTITIES> d_version;

    // Values are true for alive entities and false otherwise.
    std::array<bool, NUM_ENTITIES> d_entities;

    // Store of all components for all entities. The type of the components are erased.
    struct ComponentData
    {
        // All instances of this component.
        std::array<std::any, NUM_ENTITIES> instances;

        // Callbacks triggered whenever a new instance of this component is added. These
        // are called after the component has been added.
        std::vector<EntityCallback>      onAdd;

        // Callbacks triggers whenever an instance of this component is remove. These
        // are called before the component has been removed.
        std::vector<EntityCallback>      onRemove;
    };

    std::unordered_map<std::type_index, ComponentData> d_comps;

    void Remove(u32 entity, std::type_index type);

public:
    Registry();

    Entity New();
    void Delete(Entity entity);

    std::size_t Size() const;

    template <typename Comp> void OnAdd(const EntityCallback& cb);
    template <typename Comp> void OnRemove(const EntityCallback& cb);

    // The rest of the interface uses entity IDs rather than the entity struct.
    // It is recommended to use the corresponding functions on the entity struct.

    bool Valid(u32 entity) const;

    template <typename Comp> Comp& Add(u32 entity);
    template <typename Comp> Comp& Add(u32 entity, const Comp& component);
    template <typename Comp, typename... Args> Comp& Emplace(u32 entity, Args&&... args);
    template <typename Comp> void Remove(u32 entity);
    template <typename Comp> Comp& Get(u32 entity);
    template <typename Comp> const Comp& Get(u32 entity) const;
    template <typename Comp> bool Has(u32 entity) const;

    static u32 GetID(u16 slot, u16 version);
    static u16 GetSlot(u32 id);
    static u16 GetVersion(u32 id);

    // Iteration
    class Iterator
    {
        Registry*   d_reg;
        std::size_t d_index;

    public:
        Iterator(Registry* reg, std::size_t index) : d_reg(reg), d_index(index) {
            while (!d_reg->d_entities[d_index] && d_index < d_reg->d_next) ++d_index;
        }
        Iterator& operator++();
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
        Entity operator*();
    };

    Iterator begin() { return Iterator(this, 0); }
    Iterator end() { return Iterator(this, d_next); }

    // Views
    template <typename... Comp> class ViewType
    {
        Registry* d_reg;

    public:
        class ViewIterator
        {
            Registry* d_reg;
            Iterator  d_iter;

        public:
            ViewIterator(Registry* reg, const Iterator& iter) : d_reg(reg), d_iter(iter) {
                while (d_iter != d_reg->end() && !((*d_iter).Has<Comp>() && ...)) ++d_iter;
            }
            ViewIterator& operator++();
            bool operator==(const ViewIterator& other) const;
            bool operator!=(const ViewIterator& other) const;
            Entity operator*();
        };

        ViewType(Registry* reg) : d_reg(reg) {}

        ViewIterator begin() { return ViewIterator(d_reg, d_reg->begin()); }
        ViewIterator end() { return ViewIterator(d_reg, d_reg->end()); }
    };

    template <typename... Comp> ViewType<Comp...> View()
    {
        return ViewType<Comp...>(this);
    }

    friend class Iterator;
};

// An "empty" entity.
static const Entity Null(nullptr, NULL_ID);

// ==============================================================
//                      TEMPLATE DEFINITIONS
// ==============================================================

// REGISTRY TEMPLATES

template <typename Comp>
Comp& Registry::Add(u32 entity)
{
    Add<Comp>(entity, Comp{});
}

template <typename Comp>
Comp& Registry::Add(u32 entity, const Comp& component)
{
    assert(Valid(entity));
    auto& entry = d_comps[typeid(Comp)].instances[GetSlot(entity)];
    entry = component;

    for (const auto& cb : d_comps[typeid(Comp)].onAdd) {
        cb({this, entity});
    }

    return std::any_cast<Comp&>(entry);
}

template <typename Comp, typename... Args>
Comp& Registry::Emplace(u32 entity, Args&&... args)
{
    assert(Valid(entity));
    auto& entry = d_comps[typeid(Comp)].instances[GetSlot(entity)];
    entry = std::make_any<Comp&>(std::forward<Args>(args)...);

    for (const auto& cb : d_comps[typeid(Comp)].onAdd) {
        cb({this, entity});
    }

    return std::any_cast<Comp&>(entry);
}

template <typename Comp>
void Registry::Remove(u32 entity)
{
    assert(Valid(entity));
    Remove(entity, typeid(Comp));
}

template <typename Comp>
Comp& Registry::Get(u32 entity)
{
    assert(Valid(entity));
    auto& entry = d_comps.at(typeid(Comp)).instances.at(GetSlot(entity));
    return std::any_cast<Comp&>(entry);
}

template <typename Comp>
const Comp& Registry::Get(u32 entity) const
{
    assert(Valid(entity));
    auto& entry = d_comps.at(typeid(Comp)).instances.at(GetSlot(entity));
    return std::any_cast<Comp&>(entry);
}

template <typename Comp>
bool Registry::Has(u32 entity) const
{
    assert(Valid(entity));
    if (auto it = d_comps.find(typeid(Comp)); it != d_comps.end()) {
        const auto& entry = it->second.instances.at(GetSlot(entity));
        return entry.has_value();
    }
    return false;
}

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

// VIEWTYPE TEMPLATES

template <typename... Comp>
typename Registry::ViewType<Comp...>::ViewIterator& Registry::ViewType<Comp...>::ViewIterator::operator++()
{
    ++d_iter;
    while (d_iter != d_reg->end() && !((*d_iter).Has<Comp>() && ...)) ++d_iter;
    return *this;
}

template <typename... Comp>
bool Registry::ViewType<Comp...>::ViewIterator::operator==(const Registry::ViewType<Comp...>::ViewIterator& other) const
{
    return d_iter == other.d_iter;
}

template <typename... Comp>
bool Registry::ViewType<Comp...>::ViewIterator::operator!=(const Registry::ViewType<Comp...>::ViewIterator& other) const
{
    return !(*this == other);
}

template <typename... Comp>
Entity Registry::ViewType<Comp...>::ViewIterator::operator*()
{
    return *d_iter;
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
    return d_registry->Add<Comp>(d_id, component);
}

template <typename Comp, typename... Args>
Comp& Entity::Emplace(Args&&... args)
{
    assert(Valid());
    return d_registry->Emplace<Comp>(id, std::forward<Args>(args)...);
}

template <typename Comp>
void Entity::Remove()
{
    assert(Valid());
    d_registry->Remove<Comp>(d_id);
}

template <typename Comp>
Comp& Entity::Get()
{
    assert(Valid());
    return d_registry->Get<Comp>(d_id);
}

template <typename Comp>
const Comp& Entity::Get() const
{
    assert(Valid());
    return d_registry->Get<Comp>(d_id);
}

template <typename Comp>
bool Entity::Has() const
{
    assert(Valid());
    return d_registry->Has<Comp>(d_id);
}

}
}