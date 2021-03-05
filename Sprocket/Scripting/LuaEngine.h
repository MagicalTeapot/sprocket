#pragma once
#include "Events.h"
#include "InputProxy.h"
#include "Scene.h"
#include "Window.h"

#include <string>
#include <memory>

#include <lua.hpp>

namespace Sprocket {

class LuaEngine
{
    std::unique_ptr<lua_State, std::function<void(lua_State*)>> d_L;

    void push_value(bool val);
    void push_value(char val);
    void push_value(int val);
    void push_value(float val);
    void push_value(double val);
    void push_value(const char* val);
    void push_value(const std::string& val);
    void push_value(void* val);
    template <typename T> void push_value(T* val);
    template <typename T> void push_value(const T& val);

    void print_errors(int rc) const;

    void* allocate(std::size_t size);

public:
    LuaEngine();

    void run_script(const std::string& filename);

    // TODO: Remove
    void print_globals();

    template <typename... Args>
    void call_function(const std::string& function, Args&&... args);

    template <typename Type>
    void set_value(const std::string& name, Type&& value);

    void on_event(ev::Event& event);
};

template <typename T> void LuaEngine::push_value(T* val)
{
    push_value(static_cast<void*>(val));
}

template <typename T> void LuaEngine::push_value(const T& val)
{
    static_assert(std::is_copy_assignable_v<T>);
    static_assert(std::is_trivially_destructible_v<T>);
    *static_cast<T*>(allocate(sizeof(T))) = val;
}

template <typename... Args>
void LuaEngine::call_function(const std::string& function, Args&&... args)
{
    lua_getglobal(d_L.get(), function.c_str());
    if (!lua_isfunction(d_L.get(), -1)) {
        lua_pop(d_L.get(), -1);
        return;
    }

    (push_value(std::forward<Args>(args)), ...);

    int rc = lua_pcall(d_L.get(), sizeof...(Args), 0, 0);
    print_errors(rc);
}

template <typename Type>
void LuaEngine::set_value(const std::string& name, Type&& value)
{
    push_value(std::forward<Type>(value));
    lua_setglobal(d_L.get(), name.c_str());
}

}