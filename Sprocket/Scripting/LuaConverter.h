#pragma once
#include "Types.h"

#include "ECS.h"
#include <cstddef>
#include <string>

#include <glm/glm.hpp>

struct lua_State;

namespace Sprocket {
namespace lua {

template <typename T> struct Converter
{
    // Pops from the top of the Lua stack.
    static T pop(lua_State* L) { static_assert(false); }

    // Pushes to the top of the Lua stack. Returns the dimension.
    static int push(lua_State* L, const T& value) { static_assert(false); }
    
    // Reads at the given position in the Lua stack, advancing the pointer
    // to the position after the last element used by this type.
    static T read(lua_State* L, int index) { static_assert(false); }
};

template <> struct Converter<int>
{
    static int pop(lua_State* L);
    static int push(lua_State* L, const int& value);
    static int read(lua_State* L, int index);
};

template <> struct Converter<u32>
{
    static u32 pop(lua_State* L);
    static int push(lua_State* L, const u32& value);
    static u32 read(lua_State* L, int index);
};

template <> struct Converter<bool>
{
    static bool pop(lua_State* L);
    static int push(lua_State* L, const bool& value);
    static bool read(lua_State* L, int index);
};

template <> struct Converter<float>
{
    static float pop(lua_State* L);
    static int push(lua_State* L, const float& value);
    static float read(lua_State* L, int index);
};

template <> struct Converter<double>
{
    static double pop(lua_State* L);
    static int push(lua_State* L, const double& value);
    static double read(lua_State* L, int index);
};

template <> struct Converter<const char*>
{
    static const char* pop(lua_State* L);
    static int push(lua_State* L, const char* value);
    static const char* read(lua_State* L, int index);
};

template <> struct Converter<std::string>
{
    static std::string pop(lua_State* L);
    static int push(lua_State* L, const std::string& value);
    static std::string read(lua_State* L, int index);
};

template <> struct Converter<void*>
{
    static void* pop(lua_State* L);
    static int push(lua_State* L, void* value);
    static void* read(lua_State* L, int index);
};

template <typename T> struct Converter<T*>
{
    static T* pop(lua_State* L) {
        return (T*)Converter<void*>::pop(L);
    }
    static int push(lua_State* L, T* value) {
        return Converter<void*>::push(L, (void*)value);
    }
    static T* read(lua_State* L, int index) {
        return (T*)Converter<void*>::read(L, index);
    }
};

template <> struct Converter<spkt::entity>
{
    static spkt::entity pop(lua_State* L);
    static int push(lua_State* L, const spkt::entity& value);
    static spkt::entity read(lua_State* L, int index);
};

template <> struct Converter<spkt::identifier>
{
    static spkt::identifier pop(lua_State* L);
    static int push(lua_State* L, const spkt::identifier& value);
    static spkt::identifier read(lua_State* L, int index);
};

// Implement the pull_from for these types when we need them, otherwise we are
// guessing the API here.

template <> struct Converter<glm::vec2>
{
    static glm::vec2 pop(lua_State* L);
    static int push(lua_State* L, const glm::vec2& value);
    static glm::vec2 read(lua_State* L, int index);
};

template <> struct Converter<glm::vec3>
{
    static glm::vec3 pop(lua_State* L);
    static int push(lua_State* L, const glm::vec3& value);
    static glm::vec3 read(lua_State* L, int index);
};

}
}