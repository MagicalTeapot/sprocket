#include "Console.h"
#include <Sprocket.h>
#include <filesystem>

namespace Sprocket {

const auto LIGHT_BLUE  = Sprocket::FromHex(0x25CCF7);
const auto CLEAR_BLUE  = Sprocket::FromHex(0x1B9CFC);
const auto GARDEN      = Sprocket::FromHex(0x55E6C1);
const auto SPACE_DARK  = Sprocket::FromHex(0x2C3A47);

Console::Console(Window* window)
    : d_window(window)
    , d_ui(window)
{
    SimpleUITheme theme;
    theme.backgroundColour = SPACE_DARK;
    theme.backgroundColour.w = 0.8f;
    theme.baseColour = CLEAR_BLUE;
    theme.hoveredColour = LIGHT_BLUE;
    theme.clickedColour = GARDEN;
    d_ui.SetTheme(theme);
}

void Console::OnUpdate(double dt)
{
    d_ui.OnUpdate(dt);

    d_ui.StartFrame();
    double W = 0.8 * d_window->Width() - 20;
    double H = d_window->Height() - 20;
    Maths::vec4 mainRegion = {10, 10, W, H};
    bool active = true;
    bool draggable = false;
    bool clickable = false;
    d_ui.StartPanel("Main", &mainRegion, &active, &draggable, &clickable);

    double boxHeight = 50.0;
    d_ui.TextModifiable("Text", {10, H - 10 - boxHeight, W - 20, boxHeight}, &d_commandLine);
    while (d_consoleLines.size() > 100) {
        d_consoleLines.pop_back();
    }
    Maths::vec2 region = {10, H - 10 - boxHeight - 50};
    float fontSize = 24.0f;
    for (const auto& command : d_consoleLines) {
        d_ui.Text(command, fontSize, region);
        region.y -= fontSize;
    }

    d_ui.EndPanel();
}

void Console::OnEvent(Event& event)
{
    auto e = event.As<KeyboardButtonPressedEvent>();
    if (e && e->Key() == Keyboard::ENTER) {
        d_consoleLines.push_front(d_commandLine);
        HandleCommand(d_commandLine);
        d_commandLine = "";
        e->Consume();
    } else {
        d_ui.OnEvent(event);
    }
}

void Console::Draw()
{
    d_ui.EndFrame();
}

void Console::HandleCommand(const std::string& command)
{
    SPKT_LOG_INFO("Handled: {}", command);
    if (command == "clear") {
        d_consoleLines.clear();
    }
    else {
        Sprocket::LuaEngine engine;
        auto script = std::string("Resources/Scripts/") + command + ".lua";
        if (std::filesystem::exists(script)) {
            engine.RunScript(script);
        } else {
            d_consoleLines.push_front("Could not find script");
        }
    }
}

}