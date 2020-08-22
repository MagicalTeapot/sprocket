#pragma once
#include <Sprocket.h>
#include "EditorLayer.h"

#include <memory>

namespace Sprocket {

class EscapeMenu : public Layer
{
    WorldLayer* d_worldLayer;
    EditorUI* d_editorUi;
    
    SimpleUI d_ui;

public:
    EscapeMenu(const CoreSystems& core,
               WorldLayer* worldLayer);

    void OnEvent(Event& event) override;
    void OnUpdate(double dt) override;
};

}