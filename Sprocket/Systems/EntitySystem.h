#pragma once
#include "Entity.h"

namespace Sprocket {

class EntitySystem
{
public:
    EntitySystem() = default;
    
    virtual void updateSystem(float dt) = 0;
        // Called once per frame, this should update any system
        // speific data.

    virtual void preUpdateEntity(Entity& entity, float dt) = 0;
        // Called once per entity per frame and before the system updates.

    virtual void postUpdateEntity(Entity& entity, float dt) = 0;
        // Called once per entity per frame and after the system updates.

    virtual void registerEntity(const Entity& entity) = 0;
    virtual void deregisterEntity(const Entity& entity) = 0;
        // Register and deregister entities with this system.

private:
    // EntitySystems can not be copied.
    EntitySystem(const EntitySystem&) = delete;
    EntitySystem& operator=(const EntitySystem&) = delete;

    // EntitySystems can not be moved.
    EntitySystem(EntitySystem&&) = delete;
    EntitySystem& operator=(EntitySystem&&) = delete;
};

}