#include "LuaLibrary.h"
#include "ECS.h"
#include "Scene.h"
#include "InputProxy.h"
#include "Window.h"
#include "Components.h"
#include "Log.h"

#include <lua.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

namespace Sprocket {
namespace lua {
namespace {

Scene* GetScene(lua_State* L)
{
    lua_getglobal(L, "__scene__");
    Scene* s = nullptr;
    if (!lua_isnil(L, -1)) {
        s = (Scene*)lua_touserdata(L, -1);
    }
    lua_pop(L, 1);
    return s;
}

Window* GetWindow(lua_State* L)
{
    lua_getglobal(L, "__window__");
    Window* w = nullptr;
    if (!lua_isnil(L, -1)) {
        w = (Window*)lua_touserdata(L, -1);
    }
    lua_pop(L, 1);
    return w;
}

InputProxy* GetInput(lua_State* L)
{
    lua_getglobal(L, "__input__");
    InputProxy* ip = nullptr;
    if (!lua_isnil(L, -1)) {
        ip = (InputProxy*)lua_touserdata(L, -1);
    }
    lua_pop(L, 1);
    return ip;
}

bool CheckReturnCode(lua_State* L, int rc)
{
    if (rc != LUA_OK) {
        const char* error = lua_tostring(L, -1);
        log::error("[Lua]: {}", error);
        return false;
    }
    return true;
}

bool CheckArgCount(lua_State* L, int argc)
{
    int args = lua_gettop(L);
    if (args != argc) {
        log::error("[Lua]: Expected {} args, got {}", argc, args);
        return false;
    }
    return true;
}

}

void register_scene_functions(lua_State* L)
{
    using Generator = cppcoro::generator<ecs::Entity>;
    using Iterator = typename Generator::iterator;

    static_assert(std::is_trivially_destructible_v<Iterator>);
    
    lua_register(L, "NewEntity", [](lua_State* L) {
        if (!CheckArgCount(L, 0)) { return luaL_error(L, "Bad number of args"); }
        auto luaEntity = static_cast<ecs::Entity*>(lua_newuserdata(L, sizeof(ecs::Entity)));
        *luaEntity = GetScene(L)->Entities().New();
        return 1;
    });

    lua_register(L, "DeleteEntity", [](lua_State* L) {
        if (!CheckArgCount(L, 1)) { return luaL_error(L, "Bad number of args"); }
        auto luaEntity = static_cast<ecs::Entity*>(lua_touserdata(L, 1));
        luaEntity->Delete();
        return 0;
    });

    lua_register(L, "Each_New", [](lua_State* L) {
        if (!CheckArgCount(L, 0)) { return luaL_error(L, "Bad number of args"); }
        auto gen = new Generator(GetScene(L)->Entities().Each());
        lua_pushlightuserdata(L, static_cast<void*>(gen));
        return 1;
    });

    lua_register(L, "Each_Delete", [](lua_State* L) {
        if (!CheckArgCount(L, 1)) { return luaL_error(L, "Bad number of args"); }
        delete static_cast<Generator*>(lua_touserdata(L, 1));
        return 0;
    });

    lua_register(L, "Each_Iter_Start", [](lua_State*L) {
        if (!CheckArgCount(L, 1)) { return luaL_error(L, "Bad number of args"); }
        auto gen = static_cast<Generator*>(lua_touserdata(L, 1));

        auto iter = static_cast<Iterator*>(lua_newuserdata(L, sizeof(Iterator)));
        *iter = gen->begin();
        return 1;
    });

    lua_register(L, "Each_Iter_Valid", [](lua_State* L) {
        if (!CheckArgCount(L, 2)) { return luaL_error(L, "Bad number of args"); }
        auto gen = static_cast<Generator*>(lua_touserdata(L, 1));
        auto iter = static_cast<Iterator*>(lua_touserdata(L, 2));

        lua_pushboolean(L, *iter != gen->end());
        return 1;
    });

    lua_register(L, "Each_Iter_Next", [](lua_State* L) {
        if (!CheckArgCount(L, 1)) { return luaL_error(L, "Bad number of args"); }
        auto iter = static_cast<Iterator*>(lua_touserdata(L, 1));
        ++(*iter);
        return 0;   
    });

    lua_register(L, "Each_Iter_Get", [](lua_State* L) {
        if (!CheckArgCount(L, 1)) { return luaL_error(L, "Bad number of args"); }
        auto iterator = static_cast<Iterator*>(lua_touserdata(L, 1));

        auto luaEntity = static_cast<ecs::Entity*>(lua_newuserdata(L, sizeof(ecs::Entity)));
        *luaEntity = **iterator;
        return 1;
    });
}

void register_entity_transformation_functions(lua_State* L)
{
    lua_register(L, "Lua_SetLookAt", [](lua_State* L) {
        if (!CheckArgCount(L, 7)) { return luaL_error(L, "Bad number of args"); }

        ecs::Entity entity = *static_cast<ecs::Entity*>(lua_touserdata(L, 1));
        float px = (float)lua_tonumber(L, 2);
        float py = (float)lua_tonumber(L, 3);
        float pz = (float)lua_tonumber(L, 4);

        float tx = (float)lua_tonumber(L, 5);
        float ty = (float)lua_tonumber(L, 6);
        float tz = (float)lua_tonumber(L, 7);

        auto& tr = entity.Get<Transform3DComponent>();
        tr.position = glm::vec3(px, py, pz);
        tr.orientation = glm::conjugate(glm::quat_cast(glm::lookAt(tr.position, {tx, ty, tz}, {0.0, 1.0, 0.0})));
        return 0;
    });

    lua_register(L, "RotateY", [](lua_State* L) {
        if (!CheckArgCount(L, 2)) { return luaL_error(L, "Bad number of args"); };

        ecs::Entity entity = *static_cast<ecs::Entity*>(lua_touserdata(L, 1));
        auto& tr = entity.Get<Transform3DComponent>();

        float yaw = (float)lua_tonumber(L, 2);
        tr.orientation = glm::rotate(tr.orientation, yaw, {0, 1, 0});
        return 0;
    });

    lua_register(L, "Lua_GetForwardsDir", [](lua_State* L) {
        if (!CheckArgCount(L, 1)) { return luaL_error(L, "Bad number of args"); }

        ecs::Entity entity = *static_cast<ecs::Entity*>(lua_touserdata(L, 1));
        auto& tr = entity.Get<Transform3DComponent>();
        auto o = tr.orientation;
        
        if (entity.Has<Camera3DComponent>()) {
            auto pitch = entity.Get<Camera3DComponent>().pitch;
            o = glm::rotate(o, pitch, {1, 0, 0});
        }

        auto forwards = Maths::Forwards(o);

        lua_pushnumber(L, forwards.x);
        lua_pushnumber(L, forwards.y);
        lua_pushnumber(L, forwards.z);
        return 3;
    });

    lua_register(L, "Lua_GetRightDir", [](lua_State* L) {
        if (!CheckArgCount(L, 1)) { return luaL_error(L, "Bad number of args"); }

        ecs::Entity entity = *static_cast<ecs::Entity*>(lua_touserdata(L, 1));
        auto& tr = entity.Get<Transform3DComponent>();
        auto right = Maths::Right(tr.orientation);
        lua_pushnumber(L, right.x);
        lua_pushnumber(L, right.y);
        lua_pushnumber(L, right.z);
        return 3;
    });

    lua_register(L, "MakeUpright", [](lua_State* L) {
        if (!CheckArgCount(L, 2)) { return luaL_error(L, "Bad number of args"); }
        ecs::Entity entity = *static_cast<ecs::Entity*>(lua_touserdata(L, 1));
        auto& tr = entity.Get<Transform3DComponent>();
        float yaw = (float)lua_tonumber(L, 2);
        tr.orientation = glm::quat(glm::vec3(0, yaw, 0));
        return 0;
    });
}

void register_input_functions(lua_State* L)
{
    lua_register(L, "IsKeyDown", [](lua_State* L) {
        if (!CheckArgCount(L, 1)) { return luaL_error(L, "Bad number of args"); }

        if (auto ip = GetInput(L); ip) {
            int x = (int)lua_tointeger(L, 1);
            lua_pushboolean(L, ip->IsKeyboardDown(x));
        }
        else {
            lua_pushboolean(L, false);
        }
        
        return 1;
    });
    
    lua_register(L, "IsMouseDown", [](lua_State* L) {
        if (!CheckArgCount(L, 1)) { return luaL_error(L, "Bad number of args"); }

        if (auto ip = GetInput(L); ip) {
            int x = (int)lua_tointeger(L, 1);
            lua_pushboolean(L, ip->IsMouseDown(x));
        }
        else {
            lua_pushboolean(L, false);
        }

        return 1;
    });
    
    lua_register(L, "GetMousePos", [](lua_State* L) {
        if (!CheckArgCount(L, 0)) { return luaL_error(L, "Bad number of args"); }

        if (auto w = GetWindow(L); w) {
            auto offset = w->GetMousePos();
            lua_pushnumber(L, offset.x);
            lua_pushnumber(L, offset.y);
        } else {
            lua_pushnumber(L, 0);
            lua_pushnumber(L, 0);
        }
        return 2;
    });

    lua_register(L, "GetMouseOffset", [](lua_State* L) {
        if (!CheckArgCount(L, 0)) { return luaL_error(L, "Bad number of args"); }

        if (auto w = GetWindow(L); w) {
            auto offset = w->GetMouseOffset();
            lua_pushnumber(L, offset.x);
            lua_pushnumber(L, offset.y);
        } else {
            lua_pushnumber(L, 0);
            lua_pushnumber(L, 0);
        }
        return 2;
    });
}

namespace {

// PUSH
int Push(lua_State* L, int value)
{
    lua_pushnumber(L, value);
    return 1;
}

int Push(lua_State* L, float value)
{
    lua_pushnumber(L, value);
    return 1;
}

int Push(lua_State* L, const std::string& value)
{
    lua_pushstring(L, value.c_str());
    return 1;
}

int Push(lua_State* L, const bool& value)
{
    lua_pushboolean(L, value);
    return 1;
}

int Push(lua_State* L, const glm::vec2& value)
{
    lua_pushnumber(L, value.x);
    lua_pushnumber(L, value.y);
    return 3;
}

int Push(lua_State* L, const glm::vec3& value)
{
    lua_pushnumber(L, value.x);
    lua_pushnumber(L, value.y);
    lua_pushnumber(L, value.z);
    return 3;
}

// PULL
template <typename T> T Pull(lua_State* L, int& count)
{
    static_assert(sizeof(T) == -1);
    return T();
}

template <> int Pull(lua_State* L, int& count)
{
    return (int)lua_tonumber(L, count++);
}

template <> float Pull(lua_State* L, int& count)
{
    return (float)lua_tonumber(L, count++);
}

template <> std::string Pull(lua_State* L, int& count)
{
    return std::string(lua_tostring(L, count++));
}

template <> bool Pull(lua_State* L, int& count)
{
    return (bool)lua_toboolean(L, count++);
}

template <> glm::vec3 Pull(lua_State* L, int& count)
{
    float x = (float)lua_tonumber(L, count++);
    float y = (float)lua_tonumber(L, count++);
    float z = (float)lua_tonumber(L, count++);
    return {x, y, z};
}

template <> glm::vec2 Pull(lua_State* L, int& count)
{
    float x = (float)lua_tonumber(L, count++);
    float y = (float)lua_tonumber(L, count++);
    return {x, y};
}

template <> glm::quat Pull(lua_State* L, int& count)
{
    float x = (float)lua_tonumber(L, count++);
    float y = (float)lua_tonumber(L, count++);
    float z = (float)lua_tonumber(L, count++);
    float w = (float)lua_tonumber(L, count++);
    return {x, y, z, w};
}

// DIMENSION
template <typename T> constexpr int Dimension()
{
    static_assert(sizeof(T) == -1);
    return 0;
}

template <> constexpr int Dimension<int>() { return 1; }
template <> constexpr int Dimension<float>() { return 1; }
template <> constexpr int Dimension<bool>() { return 1; }
template <> constexpr int Dimension<std::string>() { return 1; }
template <> constexpr int Dimension<glm::vec2>() { return 2; }
template <> constexpr int Dimension<glm::vec3>() { return 3; }
template <> constexpr int Dimension<glm::quat>() { return 4; }

#ifdef DATAMATIC_BLOCK SCRIPTABLE=true
constexpr int {{Comp.Name}}Dimension()
{
    int count = 0;
    count += Dimension<{{Attr.Type}}>(); // {{Attr.Name}}
    return count;
} 

#endif

template<typename T> int Lua_Has(lua_State* L)
{
    if (!CheckArgCount(L, 1)) { return luaL_error(L, "Bad number of args"); }

    ecs::Entity entity = *static_cast<ecs::Entity*>(lua_touserdata(L, 1));
    lua_pushboolean(L, entity.Has<T>());
    return 1;
}

}

void register_entity_component_functions(lua_State* L)
{
#ifdef DATAMATIC_BLOCK SCRIPTABLE=true
    lua_register(L, "Lua_Get{{Comp.Name}}", [](lua_State* L) {
        if (!CheckArgCount(L, 1)) { return luaL_error(L, "Bad number of args"); }

        ecs::Entity e = *static_cast<ecs::Entity*>(lua_touserdata(L, 1));
        assert(e.Has<{{Comp.Name}}>());

        int count = 0;
        const auto& c = e.Get<{{Comp.Name}}>();
        count += Push(L, c.{{Attr.Name}});
        return count;
    });

    lua_register(L, "Lua_Set{{Comp.Name}}", [](lua_State* L) {
        if (!CheckArgCount(L, {{Comp.Name}}Dimension() + 1)) { return luaL_error(L, "Bad number of args"); }

        int count = 2;
        ecs::Entity e = *static_cast<ecs::Entity*>(lua_touserdata(L, 1));
        auto& c = e.Get<{{Comp.Name}}>();
        c.{{Attr.Name}} = Pull<{{Attr.Type}}>(L, count);
        return 0;
    });

    lua_register(L, "Lua_Add{{Comp.Name}}", [](lua_State* L) {
        if (!CheckArgCount(L, {{Comp.Name}}Dimension() + 1)) { return luaL_error(L, "Bad number of args"); }

        int count = 2;
        ecs::Entity e = *static_cast<ecs::Entity*>(lua_touserdata(L, 1));
        assert(!e.Has<{{Comp.Name}}>());

        {{Comp.Name}} c;
        c.{{Attr.Name}} = Pull<{{Attr.Type}}>(L, count);
        e.Add<{{Comp.Name}}>(c);
        return 0;
    });

    lua_register(L, "Has{{Comp.Name}}", &Lua_Has<{{Comp.Name}}>);

#endif
}

}
}