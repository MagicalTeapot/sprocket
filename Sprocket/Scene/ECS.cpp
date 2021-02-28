#include "ECS.h"
#include "Components.h"

#include <algorithm>
#include <random>

namespace Sprocket {
namespace ecs {

bool Entity::operator==(Entity other) const
{
    // Change this to only compare registry and guid. If both of these
    // are equal but the index is wrong, we have badly screwed something
    // up, OR have a guid clash. Should assert
    return d_registry == other.d_registry
        && d_index == other.d_index
        && d_guid == other.d_guid;
}

bool Entity::operator!=(Entity other) const
{
    return !(*this == other);
}

Entity& Entity::operator=(Entity other)
{
    d_registry = other.d_registry;
    d_index = other.d_index;
    d_guid = other.d_guid;
    return *this;
}

bool Entity::Valid() const
{
     return *this != ecs::Null
         && d_registry
         && d_registry->d_entities.Has(d_index)
         && d_registry->d_entities[d_index] == d_guid;
}

void Entity::Delete() 
{
    if (Valid()) {
        // Clean up all components
        for (auto& [type, data] : d_registry->d_comps) {
            if (Has(type)) { Remove(type); }
        }
        d_registry->d_entities.Erase(d_index);
        d_registry->d_pool.push(d_index);
    }
}

guid::GUID Entity::Id() const
{
    return d_guid;
}

void Entity::Remove(std::type_index type)
{
    assert(Valid());
    if (!Has(type)) { return; }
    
    for (const auto& cb : d_registry->d_comps[type].onRemove) {
        cb(*this);
    }

    if (auto it = d_registry->d_comps.find(type); it != d_registry->d_comps.end()) {
        if (it->second.instances.Has(d_index)) {
            it->second.instances.Erase(d_index);
        }
    }
}

bool Entity::Has(std::type_index type) const
{
    if (auto it = d_registry->d_comps.find(type); it != d_registry->d_comps.end()) {
        if (it->second.instances.Has(d_index)) {
            const auto& entry = it->second.instances[d_index];
            return entry.has_value();
        }
    }
    return false;
}

Entity Registry::New()
{
    std::size_t index = d_entities.Size();

    // If there is a slot in the pool, use that instead.
    if (!d_pool.empty()) {
        index = d_pool.front();
        d_pool.pop();
    }

    guid::GUID guid = d_generator.New();

    d_entities.Insert(index, guid);
    return {this, index, guid};
}

void Registry::DeleteAll()
{
    // Clean up components, triggering onRemove behaviour
    for (const auto& [index, version] : d_entities.Safe()) {
        Entity{this, index, version}.Delete();
    }

    // Reset all entity storage
    d_entities.Clear();
    std::queue<std::size_t>().swap(d_pool);

    // TODO: Also reset component storage without affecting OnAdd/OnRemove callbacks
}

void Registry::Clear()
{
    // Reset all components and remove all callbacks
    d_comps.clear();

    // Reset all entity storage
    d_entities.Clear();
    std::queue<std::size_t>().swap(d_pool);
}

std::size_t Registry::Size() const
{
    return d_entities.Size();
}

cppcoro::generator<Entity> Registry::Each()
{
    for (const auto& [index, guid] : d_entities.Fast()) {
        co_yield {this, index, guid};
    }
}

}
}