#pragma once
#include "apecs.hpp"
// Functions for loading C++ implemented Lua functions into the given
// lua state.

struct lua_State;

namespace spkt {
namespace lua {

class Script;

// Loads the given scene into the given lua state, and provides functions for
// creating/deleting entities as well as iterating entities. Also provides functions
// for access the singleton entity, which in turn gives access to the keyboard, mouse
// and window via the InputSingleton.
void load_registry_functions(lua::Script& script, apx::registry& registry);

// Loads a bunch of helper functions to ease entity manipulation.
void load_entity_transformation_functions(lua::Script& script);

// Loads functions for accessing and modifying components on entities. The implementation
// of this is generated by Datamatic.
void load_entity_component_functions(lua::Script& script);

}
}