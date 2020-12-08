#pragma once
#include "Types.h"

#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <array>
#include <memory>
#include <unordered_set>
#include <functional>
#include <deque>
#include <limits>
#include <vector>
#include <cassert>

namespace Sprocket {
namespace ECS {

class Registry;

struct Entity
{
    Registry* const registry;
    const u32 id;

    Entity(Registry* r, u32 i) : registry(r), id(i) {}

    bool Valid() const;

    template <typename Comp> Comp& Add(const Comp& component);
    template <typename Comp> void Remove();
    template <typename Comp> Comp& Get();
    template <typename Comp> const Comp& Get() const;
    template <typename Comp> bool Has() const;

    bool operator==(Entity other) const;
    bool operator!=(Entity other) const;
};

class Registry
{
public:
    using Opaque = std::unique_ptr<void, std::function<void(void*)>>;
    using Comparitor = std::function<bool(Entity, Entity)>;
    using EntityCallback = std::function<void(Entity)>;

private:

    union Handle
    {
        struct { u16 index; u16 version; };
        u32 entity;

        Handle(u32 e) : entity(e) {}
        Handle(u16 i, u16 v) : index(i), version(v) {}
        Handle() : entity(0) {}
    };

    static constexpr std::size_t NUM_ENTITIES = std::numeric_limits<u16>::max();

    // When an entity is removed, their ID is added to the pool so that it can be reused.
    std::unordered_set<u16> d_pool;

    // If the pool of IDs is empty, then the next entity will use this variable as their ID.
    u16 d_next;

    // We also keep track of the number of times an entity ID has been used. This is used to
    // check if a Entity handle is still valid. If the entity
    std::array<u16, NUM_ENTITIES> d_version;

    // This deque contains the currently alive entities. This is used for iteration.
    std::deque<u16> d_entities;

    // Store of all components for all entities. The type of the components are erased.
    struct ComponentData
    {
        // All instances of this component.
        std::array<Opaque, NUM_ENTITIES> instances;

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
    bool Valid(u32 entity) const;

    void Sort(const Comparitor& compare);
    template <typename Comp> void Sort(const Comparitor& compare);

    template <typename Comp> Comp& Add(u32 entity, const Comp& component);
    template <typename Comp> void Remove(u32 entity);
    template <typename Comp> Comp& Get(u32 entity);
    template <typename Comp> const Comp& Get(u32 entity) const;
    template <typename Comp> bool Has(u32 entity) const;

    template <typename Comp> void OnAdd(const EntityCallback& cb);
    template <typename Comp> void OnRemove(const EntityCallback& cb);

    // Iteration
    class Iterator
    {
        Registry*   d_reg;
        std::size_t d_dequeIndex;

    public:
        Iterator(Registry* reg, std::size_t index) : d_reg(reg), d_dequeIndex(index) {}
        Iterator& operator++();
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
        Entity operator*();
    };

    Iterator begin() { return Iterator(this, 0); }
    Iterator end() { return Iterator(this, d_entities.size()); }

    // Views
    template <typename Comp> class ViewType
    {
        Registry* d_reg;

    public:
        class ViewIterator
        {
            Registry* d_reg;
            Iterator  d_iter;

        public:
            ViewIterator(Registry* reg, const Iterator& iter) : d_reg(reg), d_iter(iter) {
                while (d_iter != d_reg->end() && !(*d_iter).Has<Comp>()) ++d_iter;
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

    template <typename Comp> ViewType<Comp> View()
    {
        return ViewType<Comp>(this);
    }

    friend class Iterator;
};

// An "empty" entity.
static const Entity Null(nullptr, std::numeric_limits<u32>::max());

// ==============================================================
//                      TEMPLATE DEFINITIONS
// ==============================================================

// REGISTRY TEMPLATES

template <typename Comp>
void Registry::Sort(const Comparitor& compare)
{
    Sort([&](Entity a, Entity b) {
        bool ac = a.Has<Comp>();
        bool bc = b.Has<Comp>();
        if (ac && bc) {
            return compare(a, b);
        }
        return ac; // Comp will shift to the front.
    });
}

template <typename Comp>
Comp& Registry::Add(u32 entity, const Comp& component)
{
    assert(Valid(entity));
    static constexpr auto Deleter = [](void* data) {
        if (data) delete static_cast<Comp*>(data);
    };

    Handle handle = entity;
    auto& entry = d_comps[typeid(Comp)].instances[handle.index];
    entry = Opaque(new Comp(component), Deleter);

    for (const auto& cb : d_comps[typeid(Comp)].onAdd) {
        cb({this, entity});
    }

    return *static_cast<Comp*>(entry.get());
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
    Handle handle = entity;
    const auto& entry = d_comps.at(typeid(Comp)).instances.at(handle.index);
    return *static_cast<Comp*>(entry.get());
}

template <typename Comp>
const Comp& Registry::Get(u32 entity) const
{
    assert(Valid(entity));
    Handle handle = entity;
    const auto& entry = d_comps.at(typeid(Comp)).instances.at(handle.index);
    return *static_cast<Comp*>(entry.get());
}

template <typename Comp>
bool Registry::Has(u32 entity) const
{
    assert(Valid(entity));
    Handle handle = entity;
    if (auto it = d_comps.find(typeid(Comp)); it != d_comps.end()) {
        const auto& entry = it->second.instances.at(handle.index);
        return entry != nullptr;
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

template <typename Comp>
typename Registry::ViewType<Comp>::ViewIterator& Registry::ViewType<Comp>::ViewIterator::operator++()
{
    ++d_iter;
    while (d_iter != d_reg->end() && !(*d_iter).Has<Comp>()) ++d_iter;
    return *this;
}

template <typename Comp>
bool Registry::ViewType<Comp>::ViewIterator::operator==(const Registry::ViewType<Comp>::ViewIterator& other) const
{
    return d_iter == other.d_iter;
}

template <typename Comp>
bool Registry::ViewType<Comp>::ViewIterator::operator!=(const Registry::ViewType<Comp>::ViewIterator& other) const
{
    return !(*this == other);
}

template <typename Comp>
Entity Registry::ViewType<Comp>::ViewIterator::operator*()
{
    return *d_iter;
}

// ENTITY TEMPLATES

template <typename Comp>
Comp& Entity::Add(const Comp& component)
{
    assert(this->Valid());
    return registry->Add<Comp>(id, component);
}

template <typename Comp>
void Entity::Remove()
{
    assert(this->Valid());
    registry->Remove<Comp>(id);
}

template <typename Comp>
Comp& Entity::Get()
{
    assert(this->Valid());
    return registry->Get<Comp>(id);
}

template <typename Comp>
const Comp& Entity::Get() const
{
    assert(this->Valid());
    return registry->Get<Comp>(id);
}

template <typename Comp>
bool Entity::Has() const
{
    assert(this->Valid());
    return registry->Has<Comp>(id);
}

}
}