#include "UIEngine.h"
#include "MouseCodes.h"
#include "Log.h"

namespace Sprocket {

UIEngine::UIEngine(KeyboardProxy* keyboard, MouseProxy* mouse)
    : d_keyboard(keyboard)
    , d_mouse(mouse)
{
}

WidgetInfo UIEngine::RegisterWidget(const std::string& name,
                                    const Maths::vec4& region)
{
    assert(d_currentPanel.has_value());
    WidgetInfo info;
    d_panelQuads[d_currentPanel.value().hash].push_back({name, region});
    std::size_t hash = std::hash<std::string>{}(name);

    if (hash == d_clicked) {
        info.mouseDown = d_clickedTime;
    }

    info.sinceUnlicked = d_time - d_unclickedTimes[hash];
    info.sinceClicked = d_time - d_clickedTimes[hash];

    if (hash == d_hovered) {
        info.mouseOver = d_hoveredTime;
    }
    
    info.sinceUnhovered = d_time - d_unhoveredTimes[hash];
    info.sinceHovered = d_time - d_unhoveredTimes[hash];

    if (d_onClick == hash) { // Consume the onCLick
        d_onClick = 0;
        info.onClick = true;
    }

    if (d_onHover == hash) { // Consume the onHover
        d_onHover = 0;
        info.onHover = true;
    }

    return info;
}

void UIEngine::StartFrame()
{
    d_panelQuads.clear();
    d_currentPanel.reset();
}

void UIEngine::EndFrame()
{
    bool foundHovered = false;
    bool foundClicked = false;

    for (const auto& [panel, quads] : d_panelQuads) {
        for (std::size_t i = quads.size(); i != 0;) {
            --i;
            const auto& quad = quads[i];
            std::size_t hash = std::hash<std::string>{}(quad.name);
            auto hovered = d_mouse->InRegion(quad.region.x, quad.region.y, quad.region.z, quad.region.w);
            auto clicked = hovered && d_mouse->IsButtonClicked(Mouse::LEFT);

            if (!foundClicked && ((d_clicked == hash) || clicked)) {
                foundClicked = true;
                if (d_clicked != hash) {
                    d_unclickedTimes[d_clicked] = d_time;
                    d_clickedTimes[hash] = d_time;
                    d_clicked = hash;
                    d_onClick = hash;
                    d_clickedTime = 0.0;
                }
            }
            
            if (!foundHovered && hovered) {
                foundHovered = true;
                if (d_hovered != hash) {
                    d_unhoveredTimes[d_hovered] = d_time;
                    d_hoveredTimes[hash] = d_time;
                    d_hovered = hash;
                    d_onHover = hash;
                    d_hoveredTime = 0.0;
                }
            }
        }
    }

    if (foundHovered == false) {
        d_hoveredTime = 0.0;
        if (d_hovered > 0) {
            d_unhoveredTimes[d_hovered] = d_time;
            d_hovered = 0;
        }
    }
}

void UIEngine::StartPanel(std::size_t panel, const Maths::vec4& region)
{
    assert(!d_currentPanel.has_value());
    d_currentPanel = PanelData{panel, region};
    d_panelQuads.emplace(panel, std::vector<QuadData>{});
}

void UIEngine::EndPanel()
{
    assert(d_currentPanel.has_value());
    d_currentPanel.reset();
}

void UIEngine::OnUpdate(double dt)
{
    d_time += dt;
    d_clickedTime += dt;
    d_hoveredTime += dt;

    if (d_mouse->IsButtonReleased(Mouse::LEFT)) {
        d_clickedTime = 0.0;
        if (d_clicked > 0) {
            d_unclickedTimes[d_clicked] = d_time;
            d_clicked = 0;
        }
    }
}

}