#include "lua_ecs.h"
#include "ecs.h"
#include "scene.h"

#include <Sprocket/Scripting/lua_converter.h>
#include <Sprocket/Scripting/lua_script.h>
#include <Sprocket/Core/log.h>
#include <Sprocket/Utility/maths.h>

#include <lua.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

#include <format>
#include <string>
#include <utility>
#include <vector>

namespace spkt::lua {

void spkt::lua::converter<game::entity>::push(lua_State* L, const game::entity& value)
{
    game::entity* handle = (game::entity*)lua_newuserdata(L, sizeof(game::entity));
    *handle = value;
    luaL_setmetatable(L, "entity_id");
}

game::entity spkt::lua::converter<game::entity>::read(lua_State* L, int index)
{
    return *(game::entity*)luaL_checkudata(L, index, "entity_id");
}

}

namespace game {
namespace {

void do_file(lua_State* L, const char* file)
{
    if (luaL_dofile(L, file)) {
        spkt::log::error("[Lua]: Could not load {}", lua_tostring(L, -1));
    }
}

template <typename T>
T* get_pointer(lua_State* L, const std::string& var_name)
{
    lua_getglobal(L, var_name.c_str());
    T* ret = nullptr;
    if (lua_islightuserdata(L, -1)) {
        ret = static_cast<T*>(lua_touserdata(L, -1));
    } else {
        spkt::log::error("Variable {} is not light user data", var_name);
    }
    lua_pop(L, 1);
    return ret;
}

bool check_arg_count(lua_State* L, int argc)
{
    int args = lua_gettop(L);
    if (args != argc) {
        spkt::log::error("[Lua]: Expected {} args, got {}", argc, args);
        return false;
    }
    return true;
}

template <typename T> int has_impl(lua_State* L)
{
    if (!check_arg_count(L, 1)) { return luaL_error(L, "Bad number of args"); }
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto entity = spkt::lua::converter<game::entity>::read(L, 1);
    spkt::lua::converter<bool>::push(L, reg.has<T>(entity));
    return 1;
}

template <typename T> int remove_impl(lua_State* L)
{
    if (!check_arg_count(L, 1)) { return luaL_error(L, "Bad number of args"); }
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, 1);
    add_command(L, [&, e]() { reg.remove<T>(e); });
    return 0;
}

void add_command(lua_State* L, const std::function<void()>& command)
{
    using command_list_t = std::vector<std::function<void()>>;
    command_list_t& command_list = *get_pointer<command_list_t>(L, "__command_list__");
    command_list.push_back(command);
}

template <typename... Comps>
int view_init(lua_State* L) {
    if (!check_arg_count(L, 0)) { return luaL_error(L, "Bad number of args"); }
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    using view_t = decltype(reg.view<Comps...>());
    using iter_t = decltype(std::declval<view_t>().begin());

    auto view = new view_t(reg.view<Comps...>());
    auto iter = new iter_t(view->begin());
    lua_pushlightuserdata(L, static_cast<void*>(view));
    lua_pushlightuserdata(L, static_cast<void*>(iter));
    return 2;
}

template <typename... Comps>
int view_next(lua_State* L) {
    if (!check_arg_count(L, 2)) { return luaL_error(L, "Bad number of args"); }
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    using view_t = decltype(reg.view<Comps...>());
    using iter_t = decltype(std::declval<view_t>().begin());

    auto& view = *static_cast<view_t*>(lua_touserdata(L, 1));
    auto& iter = *static_cast<iter_t*>(lua_touserdata(L, 2));
    if (iter != view.end()) {
        spkt::lua::converter<game::entity>::push(L, *iter);
        ++iter;
    } else {
        delete &view;
        lua_pushnil(L);
    }
    return 1;
}

std::string view_source_code(std::string_view name)
{
    return std::format(R"lua(
        function view_{0}()
            local view, iter = _view_{0}_init()
            return function() return _view_{0}_next(view, iter) end
        end
    )lua", name);
}

// COMPONENT RELATED CODE - GENERATED BY DATAMATIC
// C++ Functions for NameComponent =====================================================

int _GetNameComponent(lua_State* L) {
    if (!check_arg_count(L, 1)) { return luaL_error(L, "Bad number of args"); }
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, 1);
    const auto& c = reg.get<NameComponent>(e);
    spkt::lua::converter<std::string>::push(L, c.name);
    return 1;
}

int _SetNameComponent(lua_State* L) {
    if (!check_arg_count(L, 1 + 1)) { return luaL_error(L, "Bad number of args"); }
    int ptr = 0;
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, ++ptr);
    auto& c = reg.get<NameComponent>(e);
    c.name = spkt::lua::converter<std::string>::read(L, ++ptr);
    return 0;
}

int _AddNameComponent(lua_State* L) {
    if (!check_arg_count(L, 1 + 1)) { return luaL_error(L, "Bad number of args"); }
    int ptr = 0;
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, ++ptr);
    assert(!reg.has<NameComponent>(e));
    NameComponent c;
    c.name = spkt::lua::converter<std::string>::read(L, ++ptr);
    add_command(L, [&, e, c]() { reg.add<NameComponent>(e, c); });
    return 0;
}

// C++ Functions for Transform3DComponent =====================================================

int _GetTransform3DComponent(lua_State* L) {
    if (!check_arg_count(L, 1)) { return luaL_error(L, "Bad number of args"); }
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, 1);
    const auto& c = reg.get<Transform3DComponent>(e);
    spkt::lua::converter<glm::vec3>::push(L, c.position);
    spkt::lua::converter<glm::vec3>::push(L, c.scale);
    return 2;
}

int _SetTransform3DComponent(lua_State* L) {
    if (!check_arg_count(L, 2 + 1)) { return luaL_error(L, "Bad number of args"); }
    int ptr = 0;
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, ++ptr);
    auto& c = reg.get<Transform3DComponent>(e);
    c.position = spkt::lua::converter<glm::vec3>::read(L, ++ptr);
    c.scale = spkt::lua::converter<glm::vec3>::read(L, ++ptr);
    return 0;
}

int _AddTransform3DComponent(lua_State* L) {
    if (!check_arg_count(L, 2 + 1)) { return luaL_error(L, "Bad number of args"); }
    int ptr = 0;
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, ++ptr);
    assert(!reg.has<Transform3DComponent>(e));
    Transform3DComponent c;
    c.position = spkt::lua::converter<glm::vec3>::read(L, ++ptr);
    c.scale = spkt::lua::converter<glm::vec3>::read(L, ++ptr);
    add_command(L, [&, e, c]() { reg.add<Transform3DComponent>(e, c); });
    return 0;
}

// C++ Functions for StaticModelComponent =====================================================

int _GetStaticModelComponent(lua_State* L) {
    if (!check_arg_count(L, 1)) { return luaL_error(L, "Bad number of args"); }
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, 1);
    const auto& c = reg.get<StaticModelComponent>(e);
    spkt::lua::converter<std::string>::push(L, c.mesh);
    spkt::lua::converter<std::string>::push(L, c.material);
    return 2;
}

int _SetStaticModelComponent(lua_State* L) {
    if (!check_arg_count(L, 2 + 1)) { return luaL_error(L, "Bad number of args"); }
    int ptr = 0;
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, ++ptr);
    auto& c = reg.get<StaticModelComponent>(e);
    c.mesh = spkt::lua::converter<std::string>::read(L, ++ptr);
    c.material = spkt::lua::converter<std::string>::read(L, ++ptr);
    return 0;
}

int _AddStaticModelComponent(lua_State* L) {
    if (!check_arg_count(L, 2 + 1)) { return luaL_error(L, "Bad number of args"); }
    int ptr = 0;
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, ++ptr);
    assert(!reg.has<StaticModelComponent>(e));
    StaticModelComponent c;
    c.mesh = spkt::lua::converter<std::string>::read(L, ++ptr);
    c.material = spkt::lua::converter<std::string>::read(L, ++ptr);
    add_command(L, [&, e, c]() { reg.add<StaticModelComponent>(e, c); });
    return 0;
}

// C++ Functions for AnimatedModelComponent =====================================================

int _GetAnimatedModelComponent(lua_State* L) {
    if (!check_arg_count(L, 1)) { return luaL_error(L, "Bad number of args"); }
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, 1);
    const auto& c = reg.get<AnimatedModelComponent>(e);
    spkt::lua::converter<std::string>::push(L, c.mesh);
    spkt::lua::converter<std::string>::push(L, c.material);
    spkt::lua::converter<std::string>::push(L, c.animation_name);
    spkt::lua::converter<float>::push(L, c.animation_time);
    spkt::lua::converter<float>::push(L, c.animation_speed);
    return 5;
}

int _SetAnimatedModelComponent(lua_State* L) {
    if (!check_arg_count(L, 5 + 1)) { return luaL_error(L, "Bad number of args"); }
    int ptr = 0;
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, ++ptr);
    auto& c = reg.get<AnimatedModelComponent>(e);
    c.mesh = spkt::lua::converter<std::string>::read(L, ++ptr);
    c.material = spkt::lua::converter<std::string>::read(L, ++ptr);
    c.animation_name = spkt::lua::converter<std::string>::read(L, ++ptr);
    c.animation_time = spkt::lua::converter<float>::read(L, ++ptr);
    c.animation_speed = spkt::lua::converter<float>::read(L, ++ptr);
    return 0;
}

int _AddAnimatedModelComponent(lua_State* L) {
    if (!check_arg_count(L, 5 + 1)) { return luaL_error(L, "Bad number of args"); }
    int ptr = 0;
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, ++ptr);
    assert(!reg.has<AnimatedModelComponent>(e));
    AnimatedModelComponent c;
    c.mesh = spkt::lua::converter<std::string>::read(L, ++ptr);
    c.material = spkt::lua::converter<std::string>::read(L, ++ptr);
    c.animation_name = spkt::lua::converter<std::string>::read(L, ++ptr);
    c.animation_time = spkt::lua::converter<float>::read(L, ++ptr);
    c.animation_speed = spkt::lua::converter<float>::read(L, ++ptr);
    add_command(L, [&, e, c]() { reg.add<AnimatedModelComponent>(e, c); });
    return 0;
}

// C++ Functions for ScriptComponent =====================================================

int _GetScriptComponent(lua_State* L) {
    if (!check_arg_count(L, 1)) { return luaL_error(L, "Bad number of args"); }
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, 1);
    const auto& c = reg.get<ScriptComponent>(e);
    spkt::lua::converter<std::string>::push(L, c.script);
    spkt::lua::converter<bool>::push(L, c.active);
    return 2;
}

int _SetScriptComponent(lua_State* L) {
    if (!check_arg_count(L, 2 + 1)) { return luaL_error(L, "Bad number of args"); }
    int ptr = 0;
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, ++ptr);
    auto& c = reg.get<ScriptComponent>(e);
    c.script = spkt::lua::converter<std::string>::read(L, ++ptr);
    c.active = spkt::lua::converter<bool>::read(L, ++ptr);
    return 0;
}

int _AddScriptComponent(lua_State* L) {
    if (!check_arg_count(L, 2 + 1)) { return luaL_error(L, "Bad number of args"); }
    int ptr = 0;
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, ++ptr);
    assert(!reg.has<ScriptComponent>(e));
    ScriptComponent c;
    c.script = spkt::lua::converter<std::string>::read(L, ++ptr);
    c.active = spkt::lua::converter<bool>::read(L, ++ptr);
    add_command(L, [&, e, c]() { reg.add<ScriptComponent>(e, c); });
    return 0;
}

// C++ Functions for Camera3DComponent =====================================================

int _GetCamera3DComponent(lua_State* L) {
    if (!check_arg_count(L, 1)) { return luaL_error(L, "Bad number of args"); }
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, 1);
    const auto& c = reg.get<Camera3DComponent>(e);
    spkt::lua::converter<float>::push(L, c.fov);
    spkt::lua::converter<float>::push(L, c.pitch);
    return 2;
}

int _SetCamera3DComponent(lua_State* L) {
    if (!check_arg_count(L, 2 + 1)) { return luaL_error(L, "Bad number of args"); }
    int ptr = 0;
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, ++ptr);
    auto& c = reg.get<Camera3DComponent>(e);
    c.fov = spkt::lua::converter<float>::read(L, ++ptr);
    c.pitch = spkt::lua::converter<float>::read(L, ++ptr);
    return 0;
}

int _AddCamera3DComponent(lua_State* L) {
    if (!check_arg_count(L, 2 + 1)) { return luaL_error(L, "Bad number of args"); }
    int ptr = 0;
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, ++ptr);
    assert(!reg.has<Camera3DComponent>(e));
    Camera3DComponent c;
    c.fov = spkt::lua::converter<float>::read(L, ++ptr);
    c.pitch = spkt::lua::converter<float>::read(L, ++ptr);
    add_command(L, [&, e, c]() { reg.add<Camera3DComponent>(e, c); });
    return 0;
}

// C++ Functions for PathComponent =====================================================

int _GetPathComponent(lua_State* L) {
    if (!check_arg_count(L, 1)) { return luaL_error(L, "Bad number of args"); }
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, 1);
    const auto& c = reg.get<PathComponent>(e);
    spkt::lua::converter<float>::push(L, c.speed);
    return 1;
}

int _SetPathComponent(lua_State* L) {
    if (!check_arg_count(L, 1 + 1)) { return luaL_error(L, "Bad number of args"); }
    int ptr = 0;
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, ++ptr);
    auto& c = reg.get<PathComponent>(e);
    c.speed = spkt::lua::converter<float>::read(L, ++ptr);
    return 0;
}

int _AddPathComponent(lua_State* L) {
    if (!check_arg_count(L, 1 + 1)) { return luaL_error(L, "Bad number of args"); }
    int ptr = 0;
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, ++ptr);
    assert(!reg.has<PathComponent>(e));
    PathComponent c;
    c.speed = spkt::lua::converter<float>::read(L, ++ptr);
    add_command(L, [&, e, c]() { reg.add<PathComponent>(e, c); });
    return 0;
}

// C++ Functions for LightComponent =====================================================

int _GetLightComponent(lua_State* L) {
    if (!check_arg_count(L, 1)) { return luaL_error(L, "Bad number of args"); }
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, 1);
    const auto& c = reg.get<LightComponent>(e);
    spkt::lua::converter<glm::vec3>::push(L, c.colour);
    spkt::lua::converter<float>::push(L, c.brightness);
    return 2;
}

int _SetLightComponent(lua_State* L) {
    if (!check_arg_count(L, 2 + 1)) { return luaL_error(L, "Bad number of args"); }
    int ptr = 0;
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, ++ptr);
    auto& c = reg.get<LightComponent>(e);
    c.colour = spkt::lua::converter<glm::vec3>::read(L, ++ptr);
    c.brightness = spkt::lua::converter<float>::read(L, ++ptr);
    return 0;
}

int _AddLightComponent(lua_State* L) {
    if (!check_arg_count(L, 2 + 1)) { return luaL_error(L, "Bad number of args"); }
    int ptr = 0;
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, ++ptr);
    assert(!reg.has<LightComponent>(e));
    LightComponent c;
    c.colour = spkt::lua::converter<glm::vec3>::read(L, ++ptr);
    c.brightness = spkt::lua::converter<float>::read(L, ++ptr);
    add_command(L, [&, e, c]() { reg.add<LightComponent>(e, c); });
    return 0;
}

// C++ Functions for SunComponent =====================================================

int _GetSunComponent(lua_State* L) {
    if (!check_arg_count(L, 1)) { return luaL_error(L, "Bad number of args"); }
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, 1);
    const auto& c = reg.get<SunComponent>(e);
    spkt::lua::converter<glm::vec3>::push(L, c.colour);
    spkt::lua::converter<float>::push(L, c.brightness);
    spkt::lua::converter<glm::vec3>::push(L, c.direction);
    spkt::lua::converter<bool>::push(L, c.shadows);
    return 4;
}

int _SetSunComponent(lua_State* L) {
    if (!check_arg_count(L, 4 + 1)) { return luaL_error(L, "Bad number of args"); }
    int ptr = 0;
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, ++ptr);
    auto& c = reg.get<SunComponent>(e);
    c.colour = spkt::lua::converter<glm::vec3>::read(L, ++ptr);
    c.brightness = spkt::lua::converter<float>::read(L, ++ptr);
    c.direction = spkt::lua::converter<glm::vec3>::read(L, ++ptr);
    c.shadows = spkt::lua::converter<bool>::read(L, ++ptr);
    return 0;
}

int _AddSunComponent(lua_State* L) {
    if (!check_arg_count(L, 4 + 1)) { return luaL_error(L, "Bad number of args"); }
    int ptr = 0;
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, ++ptr);
    assert(!reg.has<SunComponent>(e));
    SunComponent c;
    c.colour = spkt::lua::converter<glm::vec3>::read(L, ++ptr);
    c.brightness = spkt::lua::converter<float>::read(L, ++ptr);
    c.direction = spkt::lua::converter<glm::vec3>::read(L, ++ptr);
    c.shadows = spkt::lua::converter<bool>::read(L, ++ptr);
    add_command(L, [&, e, c]() { reg.add<SunComponent>(e, c); });
    return 0;
}

// C++ Functions for AmbienceComponent =====================================================

int _GetAmbienceComponent(lua_State* L) {
    if (!check_arg_count(L, 1)) { return luaL_error(L, "Bad number of args"); }
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, 1);
    const auto& c = reg.get<AmbienceComponent>(e);
    spkt::lua::converter<glm::vec3>::push(L, c.colour);
    spkt::lua::converter<float>::push(L, c.brightness);
    return 2;
}

int _SetAmbienceComponent(lua_State* L) {
    if (!check_arg_count(L, 2 + 1)) { return luaL_error(L, "Bad number of args"); }
    int ptr = 0;
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, ++ptr);
    auto& c = reg.get<AmbienceComponent>(e);
    c.colour = spkt::lua::converter<glm::vec3>::read(L, ++ptr);
    c.brightness = spkt::lua::converter<float>::read(L, ++ptr);
    return 0;
}

int _AddAmbienceComponent(lua_State* L) {
    if (!check_arg_count(L, 2 + 1)) { return luaL_error(L, "Bad number of args"); }
    int ptr = 0;
    auto& reg = *get_pointer<game::registry>(L, "__registry__");
    auto e = spkt::lua::converter<game::entity>::read(L, ++ptr);
    assert(!reg.has<AmbienceComponent>(e));
    AmbienceComponent c;
    c.colour = spkt::lua::converter<glm::vec3>::read(L, ++ptr);
    c.brightness = spkt::lua::converter<float>::read(L, ++ptr);
    add_command(L, [&, e, c]() { reg.add<AmbienceComponent>(e, c); });
    return 0;
}


void load_entity_transformation_functions(spkt::lua::script& script)
{
    lua_State* L = script.native_handle();

    lua_register(L, "SetLookAt", [](lua_State* L) {
        if (!check_arg_count(L, 3)) { return luaL_error(L, "Bad number of args"); }
        auto& reg = *get_pointer<game::registry>(L, "__registry__");
        auto e = spkt::lua::converter<game::entity>::read(L, 1);
        glm::vec3 p = spkt::lua::converter<glm::vec3>::read(L, 2);
        glm::vec3 t = spkt::lua::converter<glm::vec3>::read(L, 3);
        auto& tr = reg.get<Transform3DComponent>(e);
        tr.position = p;
        tr.orientation = glm::conjugate(glm::quat_cast(glm::lookAt(tr.position, t, {0.0, 1.0, 0.0})));
        return 0;
    });

    lua_register(L, "RotateY", [](lua_State* L) {
        if (!check_arg_count(L, 2)) { return luaL_error(L, "Bad number of args"); };
        auto& reg = *get_pointer<game::registry>(L, "__registry__");
        auto e = spkt::lua::converter<game::entity>::read(L, 1);
        auto& tr = reg.get<Transform3DComponent>(e);
        float yaw = (float)lua_tonumber(L, 2);
        tr.orientation = glm::rotate(tr.orientation, yaw, {0, 1, 0});
        return 0;
    });

    lua_register(L, "GetForwardsDir", [](lua_State* L) {
        if (!check_arg_count(L, 1)) { return luaL_error(L, "Bad number of args"); }
        auto& reg = *get_pointer<game::registry>(L, "__registry__");
        auto e = spkt::lua::converter<game::entity>::read(L, 1);
        auto& tr = reg.get<Transform3DComponent>(e);
        auto o = tr.orientation;

        if (auto* pc = reg.get_if<Camera3DComponent>(e)) {
            o = glm::rotate(o, pc->pitch, {1, 0, 0});
        }

        spkt::lua::converter<glm::vec3>::push(L, spkt::forwards(o));
        return 1;
    });

    lua_register(L, "GetRightDir", [](lua_State* L) {
        if (!check_arg_count(L, 1)) { return luaL_error(L, "Bad number of args"); }
        auto& reg = *get_pointer<game::registry>(L, "__registry__");
        auto e = spkt::lua::converter<game::entity>::read(L, 1);
        auto& tr = reg.get<Transform3DComponent>(e);
        spkt::lua::converter<glm::vec3>::push(L, spkt::right(tr.orientation));
        return 1;
    });

    lua_register(L, "MakeUpright", [](lua_State* L) {
        if (!check_arg_count(L, 2)) { return luaL_error(L, "Bad number of args"); }
        auto& reg = *get_pointer<game::registry>(L, "__registry__");
        auto e = spkt::lua::converter<game::entity>::read(L, 1);
        auto& tr = reg.get<Transform3DComponent>(e);
        float yaw = spkt::lua::converter<float>::read(L, 2);
        tr.orientation = glm::quat(glm::vec3(0, yaw, 0));
        return 0;
    });
}

void load_entity_component_functions(spkt::lua::script& script)
{
    lua_State* L = script.native_handle();

    // Lua functions for NameComponent =====================================================

    luaL_dostring(L, R"lua(
        NameComponent = Class(function(self, name)
            self.name = name
        end)
    )lua");

    lua_register(L, "_GetNameComponent", &_GetNameComponent);

    luaL_dostring(L, R"lua(
        function GetNameComponent(entity)
            name = _GetNameComponent(entity)
            return NameComponent(name)
        end
    )lua");

    lua_register(L, "_SetNameComponent", &_SetNameComponent);

    luaL_dostring(L, R"lua(
        function SetNameComponent(entity, c)
            _SetNameComponent(entity, c.name)
        end
    )lua");

    lua_register(L, "_AddNameComponent", &_AddNameComponent);

    luaL_dostring(L, R"lua(
        function AddNameComponent(entity, c)
            _AddNameComponent(entity, c.name)
        end
    )lua");

    lua_register(L, "HasNameComponent", &has_impl<NameComponent>);
    lua_register(L, "RemoveNameComponent", &remove_impl<NameComponent>);


    // Lua functions for Transform3DComponent =====================================================

    luaL_dostring(L, R"lua(
        Transform3DComponent = Class(function(self, position, scale)
            self.position = position
            self.scale = scale
        end)
    )lua");

    lua_register(L, "_GetTransform3DComponent", &_GetTransform3DComponent);

    luaL_dostring(L, R"lua(
        function GetTransform3DComponent(entity)
            position, scale = _GetTransform3DComponent(entity)
            return Transform3DComponent(position, scale)
        end
    )lua");

    lua_register(L, "_SetTransform3DComponent", &_SetTransform3DComponent);

    luaL_dostring(L, R"lua(
        function SetTransform3DComponent(entity, c)
            _SetTransform3DComponent(entity, c.position, c.scale)
        end
    )lua");

    lua_register(L, "_AddTransform3DComponent", &_AddTransform3DComponent);

    luaL_dostring(L, R"lua(
        function AddTransform3DComponent(entity, c)
            _AddTransform3DComponent(entity, c.position, c.scale)
        end
    )lua");

    lua_register(L, "HasTransform3DComponent", &has_impl<Transform3DComponent>);
    lua_register(L, "RemoveTransform3DComponent", &remove_impl<Transform3DComponent>);


    // Lua functions for StaticModelComponent =====================================================

    luaL_dostring(L, R"lua(
        StaticModelComponent = Class(function(self, mesh, material)
            self.mesh = mesh
            self.material = material
        end)
    )lua");

    lua_register(L, "_GetStaticModelComponent", &_GetStaticModelComponent);

    luaL_dostring(L, R"lua(
        function GetStaticModelComponent(entity)
            mesh, material = _GetStaticModelComponent(entity)
            return StaticModelComponent(mesh, material)
        end
    )lua");

    lua_register(L, "_SetStaticModelComponent", &_SetStaticModelComponent);

    luaL_dostring(L, R"lua(
        function SetStaticModelComponent(entity, c)
            _SetStaticModelComponent(entity, c.mesh, c.material)
        end
    )lua");

    lua_register(L, "_AddStaticModelComponent", &_AddStaticModelComponent);

    luaL_dostring(L, R"lua(
        function AddStaticModelComponent(entity, c)
            _AddStaticModelComponent(entity, c.mesh, c.material)
        end
    )lua");

    lua_register(L, "HasStaticModelComponent", &has_impl<StaticModelComponent>);
    lua_register(L, "RemoveStaticModelComponent", &remove_impl<StaticModelComponent>);


    // Lua functions for AnimatedModelComponent =====================================================

    luaL_dostring(L, R"lua(
        AnimatedModelComponent = Class(function(self, mesh, material, animation_name, animation_time, animation_speed)
            self.mesh = mesh
            self.material = material
            self.animation_name = animation_name
            self.animation_time = animation_time
            self.animation_speed = animation_speed
        end)
    )lua");

    lua_register(L, "_GetAnimatedModelComponent", &_GetAnimatedModelComponent);

    luaL_dostring(L, R"lua(
        function GetAnimatedModelComponent(entity)
            mesh, material, animation_name, animation_time, animation_speed = _GetAnimatedModelComponent(entity)
            return AnimatedModelComponent(mesh, material, animation_name, animation_time, animation_speed)
        end
    )lua");

    lua_register(L, "_SetAnimatedModelComponent", &_SetAnimatedModelComponent);

    luaL_dostring(L, R"lua(
        function SetAnimatedModelComponent(entity, c)
            _SetAnimatedModelComponent(entity, c.mesh, c.material, c.animation_name, c.animation_time, c.animation_speed)
        end
    )lua");

    lua_register(L, "_AddAnimatedModelComponent", &_AddAnimatedModelComponent);

    luaL_dostring(L, R"lua(
        function AddAnimatedModelComponent(entity, c)
            _AddAnimatedModelComponent(entity, c.mesh, c.material, c.animation_name, c.animation_time, c.animation_speed)
        end
    )lua");

    lua_register(L, "HasAnimatedModelComponent", &has_impl<AnimatedModelComponent>);
    lua_register(L, "RemoveAnimatedModelComponent", &remove_impl<AnimatedModelComponent>);


    // Lua functions for ScriptComponent =====================================================

    luaL_dostring(L, R"lua(
        ScriptComponent = Class(function(self, script, active)
            self.script = script
            self.active = active
        end)
    )lua");

    lua_register(L, "_GetScriptComponent", &_GetScriptComponent);

    luaL_dostring(L, R"lua(
        function GetScriptComponent(entity)
            script, active = _GetScriptComponent(entity)
            return ScriptComponent(script, active)
        end
    )lua");

    lua_register(L, "_SetScriptComponent", &_SetScriptComponent);

    luaL_dostring(L, R"lua(
        function SetScriptComponent(entity, c)
            _SetScriptComponent(entity, c.script, c.active)
        end
    )lua");

    lua_register(L, "_AddScriptComponent", &_AddScriptComponent);

    luaL_dostring(L, R"lua(
        function AddScriptComponent(entity, c)
            _AddScriptComponent(entity, c.script, c.active)
        end
    )lua");

    lua_register(L, "HasScriptComponent", &has_impl<ScriptComponent>);
    lua_register(L, "RemoveScriptComponent", &remove_impl<ScriptComponent>);


    // Lua functions for Camera3DComponent =====================================================

    luaL_dostring(L, R"lua(
        Camera3DComponent = Class(function(self, fov, pitch)
            self.fov = fov
            self.pitch = pitch
        end)
    )lua");

    lua_register(L, "_GetCamera3DComponent", &_GetCamera3DComponent);

    luaL_dostring(L, R"lua(
        function GetCamera3DComponent(entity)
            fov, pitch = _GetCamera3DComponent(entity)
            return Camera3DComponent(fov, pitch)
        end
    )lua");

    lua_register(L, "_SetCamera3DComponent", &_SetCamera3DComponent);

    luaL_dostring(L, R"lua(
        function SetCamera3DComponent(entity, c)
            _SetCamera3DComponent(entity, c.fov, c.pitch)
        end
    )lua");

    lua_register(L, "_AddCamera3DComponent", &_AddCamera3DComponent);

    luaL_dostring(L, R"lua(
        function AddCamera3DComponent(entity, c)
            _AddCamera3DComponent(entity, c.fov, c.pitch)
        end
    )lua");

    lua_register(L, "HasCamera3DComponent", &has_impl<Camera3DComponent>);
    lua_register(L, "RemoveCamera3DComponent", &remove_impl<Camera3DComponent>);


    // Lua functions for PathComponent =====================================================

    luaL_dostring(L, R"lua(
        PathComponent = Class(function(self, speed)
            self.speed = speed
        end)
    )lua");

    lua_register(L, "_GetPathComponent", &_GetPathComponent);

    luaL_dostring(L, R"lua(
        function GetPathComponent(entity)
            speed = _GetPathComponent(entity)
            return PathComponent(speed)
        end
    )lua");

    lua_register(L, "_SetPathComponent", &_SetPathComponent);

    luaL_dostring(L, R"lua(
        function SetPathComponent(entity, c)
            _SetPathComponent(entity, c.speed)
        end
    )lua");

    lua_register(L, "_AddPathComponent", &_AddPathComponent);

    luaL_dostring(L, R"lua(
        function AddPathComponent(entity, c)
            _AddPathComponent(entity, c.speed)
        end
    )lua");

    lua_register(L, "HasPathComponent", &has_impl<PathComponent>);
    lua_register(L, "RemovePathComponent", &remove_impl<PathComponent>);


    // Lua functions for LightComponent =====================================================

    luaL_dostring(L, R"lua(
        LightComponent = Class(function(self, colour, brightness)
            self.colour = colour
            self.brightness = brightness
        end)
    )lua");

    lua_register(L, "_GetLightComponent", &_GetLightComponent);

    luaL_dostring(L, R"lua(
        function GetLightComponent(entity)
            colour, brightness = _GetLightComponent(entity)
            return LightComponent(colour, brightness)
        end
    )lua");

    lua_register(L, "_SetLightComponent", &_SetLightComponent);

    luaL_dostring(L, R"lua(
        function SetLightComponent(entity, c)
            _SetLightComponent(entity, c.colour, c.brightness)
        end
    )lua");

    lua_register(L, "_AddLightComponent", &_AddLightComponent);

    luaL_dostring(L, R"lua(
        function AddLightComponent(entity, c)
            _AddLightComponent(entity, c.colour, c.brightness)
        end
    )lua");

    lua_register(L, "HasLightComponent", &has_impl<LightComponent>);
    lua_register(L, "RemoveLightComponent", &remove_impl<LightComponent>);


    // Lua functions for SunComponent =====================================================

    luaL_dostring(L, R"lua(
        SunComponent = Class(function(self, colour, brightness, direction, shadows)
            self.colour = colour
            self.brightness = brightness
            self.direction = direction
            self.shadows = shadows
        end)
    )lua");

    lua_register(L, "_GetSunComponent", &_GetSunComponent);

    luaL_dostring(L, R"lua(
        function GetSunComponent(entity)
            colour, brightness, direction, shadows = _GetSunComponent(entity)
            return SunComponent(colour, brightness, direction, shadows)
        end
    )lua");

    lua_register(L, "_SetSunComponent", &_SetSunComponent);

    luaL_dostring(L, R"lua(
        function SetSunComponent(entity, c)
            _SetSunComponent(entity, c.colour, c.brightness, c.direction, c.shadows)
        end
    )lua");

    lua_register(L, "_AddSunComponent", &_AddSunComponent);

    luaL_dostring(L, R"lua(
        function AddSunComponent(entity, c)
            _AddSunComponent(entity, c.colour, c.brightness, c.direction, c.shadows)
        end
    )lua");

    lua_register(L, "HasSunComponent", &has_impl<SunComponent>);
    lua_register(L, "RemoveSunComponent", &remove_impl<SunComponent>);


    // Lua functions for AmbienceComponent =====================================================

    luaL_dostring(L, R"lua(
        AmbienceComponent = Class(function(self, colour, brightness)
            self.colour = colour
            self.brightness = brightness
        end)
    )lua");

    lua_register(L, "_GetAmbienceComponent", &_GetAmbienceComponent);

    luaL_dostring(L, R"lua(
        function GetAmbienceComponent(entity)
            colour, brightness = _GetAmbienceComponent(entity)
            return AmbienceComponent(colour, brightness)
        end
    )lua");

    lua_register(L, "_SetAmbienceComponent", &_SetAmbienceComponent);

    luaL_dostring(L, R"lua(
        function SetAmbienceComponent(entity, c)
            _SetAmbienceComponent(entity, c.colour, c.brightness)
        end
    )lua");

    lua_register(L, "_AddAmbienceComponent", &_AddAmbienceComponent);

    luaL_dostring(L, R"lua(
        function AddAmbienceComponent(entity, c)
            _AddAmbienceComponent(entity, c.colour, c.brightness)
        end
    )lua");

    lua_register(L, "HasAmbienceComponent", &has_impl<AmbienceComponent>);
    lua_register(L, "RemoveAmbienceComponent", &remove_impl<AmbienceComponent>);


}

void load_entity_functions(spkt::lua::script& script)
{
    lua_State* L = script.native_handle();

    luaL_newmetatable(L, "entity_id");

    lua_pushcfunction(L, [](lua_State* L) {
        game::entity* self = (game::entity*)luaL_checkudata(L, 1, "entity_id");
        game::entity* other = (game::entity*)luaL_checkudata(L, 2, "entity_id");
        spkt::lua::converter<bool>::push(L, *self == *other);
        return 1;
    });
    lua_setfield(L, -2, "__eq");

    lua_setglobal(L, "entity_id");
}

}

void load_registry(spkt::lua::script& script, game::registry& registry)
{
    using namespace spkt;

    lua_State* L = script.native_handle();
    script.set_value("__registry__", &registry);

    load_entity_transformation_functions(script);
    load_entity_component_functions(script);
    load_entity_functions(script);

    // Add functions for creating and destroying entities.
    lua_register(L, "NewEntity", [](lua_State* L) {
        if (!check_arg_count(L, 0)) { return luaL_error(L, "Bad number of args"); }
        auto& reg = *get_pointer<game::registry>(L, "__registry__");
        spkt::lua::converter<game::entity>::push(L, reg.create());
        return 1;
    });

    lua_register(L, "DeleteEntity", [](lua_State* L) {
        if (!check_arg_count(L, 1)) { return luaL_error(L, "Bad number of args"); }
        auto& reg = *get_pointer<game::registry>(L, "__registry__");
        auto e = spkt::lua::converter<game::entity>::read(L, 1);
        add_command(L, [&, e]() { reg.destroy(e); });
        return 0;
    });

    // Add functions for iterating over all entities in __scene__. The C++ functions
    // should not be used directly, instead they should be used via the Scene:Each
    // function implemented last in Lua.
    lua_register(L, "_view_All_init", view_init<>);
    lua_register(L, "_view_All_next", view_next<>);
    luaL_dostring(L, view_source_code("All").c_str());

    // VIEW FUNCTIONS - GENERATED BY DATAMATIC
    lua_register(L, "_view_NameComponent_init", view_init<NameComponent>);
    lua_register(L, "_view_NameComponent_next", view_next<NameComponent>);
    luaL_dostring(L, view_source_code("NameComponent").c_str());

    lua_register(L, "_view_Transform3DComponent_init", view_init<Transform3DComponent>);
    lua_register(L, "_view_Transform3DComponent_next", view_next<Transform3DComponent>);
    luaL_dostring(L, view_source_code("Transform3DComponent").c_str());

    lua_register(L, "_view_StaticModelComponent_init", view_init<StaticModelComponent>);
    lua_register(L, "_view_StaticModelComponent_next", view_next<StaticModelComponent>);
    luaL_dostring(L, view_source_code("StaticModelComponent").c_str());

    lua_register(L, "_view_AnimatedModelComponent_init", view_init<AnimatedModelComponent>);
    lua_register(L, "_view_AnimatedModelComponent_next", view_next<AnimatedModelComponent>);
    luaL_dostring(L, view_source_code("AnimatedModelComponent").c_str());

    lua_register(L, "_view_ScriptComponent_init", view_init<ScriptComponent>);
    lua_register(L, "_view_ScriptComponent_next", view_next<ScriptComponent>);
    luaL_dostring(L, view_source_code("ScriptComponent").c_str());

    lua_register(L, "_view_Camera3DComponent_init", view_init<Camera3DComponent>);
    lua_register(L, "_view_Camera3DComponent_next", view_next<Camera3DComponent>);
    luaL_dostring(L, view_source_code("Camera3DComponent").c_str());

    lua_register(L, "_view_PathComponent_init", view_init<PathComponent>);
    lua_register(L, "_view_PathComponent_next", view_next<PathComponent>);
    luaL_dostring(L, view_source_code("PathComponent").c_str());

    lua_register(L, "_view_LightComponent_init", view_init<LightComponent>);
    lua_register(L, "_view_LightComponent_next", view_next<LightComponent>);
    luaL_dostring(L, view_source_code("LightComponent").c_str());

    lua_register(L, "_view_SunComponent_init", view_init<SunComponent>);
    lua_register(L, "_view_SunComponent_next", view_next<SunComponent>);
    luaL_dostring(L, view_source_code("SunComponent").c_str());

    lua_register(L, "_view_AmbienceComponent_init", view_init<AmbienceComponent>);
    lua_register(L, "_view_AmbienceComponent_next", view_next<AmbienceComponent>);
    luaL_dostring(L, view_source_code("AmbienceComponent").c_str());

}

}
