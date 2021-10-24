#pragma once
#include "ecs.h"

#include <sprocket/Core/events.h>

#include <vector>
#include <functional>

namespace game {

class event;

struct scene
{
    using system        = std::function<void(registry&, double)>;
    using event_handler = std::function<void(registry&, spkt::event&)>;

    registry                   registry;
    std::vector<system>        systems;
    std::vector<event_handler> event_handlers;

    void on_update(double dt);
    void on_event(spkt::event& event);
};

}