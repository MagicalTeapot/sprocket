#include "ScriptRunner.h"
#include "Log.h"
#include "LuaEngine.h"

#include <utility>

namespace Sprocket {

ScriptRunner::ScriptRunner()
{
    d_keyboard.ConsumeAll(false);
}

void ScriptRunner::UpdateEntity(double dt, Entity& entity)
{
    if (!entity.Has<ScriptComponent>()) {
        return;
    }

    auto& luaEngine = d_engines[entity.Id()];
    luaEngine.CallOnUpdateFunction(dt);
}

void ScriptRunner::UpdateSystem(double dt)
{
    d_mouse.OnUpdate();
}

void ScriptRunner::OnEvent(Event& event)
{
    d_keyboard.OnEvent(event);
    d_mouse.OnEvent(event);

    if (auto e = event.As<MouseButtonPressedEvent>()) {
        for (auto& [id, luaEngine] : d_engines) {
            luaEngine.CallOnMouseButtonPressedEvent(e);
        }
    }
    else if (auto e = event.As<MouseScrolledEvent>()) {
        for (auto& [id, luaEngine] : d_engines) {
            luaEngine.CallOnMouseScrolledEvent(e);
        }
    }
}

void ScriptRunner::RegisterEntity(const Entity& entity)
{
    if (!entity.Has<ScriptComponent>()) { return; }
    auto& luaEngine = d_engines[entity.Id()];
    luaEngine.SetKeyboard(&d_keyboard);
    luaEngine.SetMouse(&d_mouse);
    luaEngine.SetEntity(entity);
    luaEngine.RunScript(entity.Get<ScriptComponent>().script);
}

void ScriptRunner::DeregisterEntity(const Entity& entity)
{
    if (!entity.Has<ScriptComponent>()) { return; }
    d_engines.erase(entity.Id());
}

}