#pragma once
#include <Sprocket.h>

#include "WorldLayer.h"

class EditorUI : public Sprocket::Layer
{
    WorldLayer* d_worldLayer;

    Sprocket::DevGUI d_editorUI;
    Sprocket::DevGUIRenderer d_editorUIRenderer;

    Sprocket::ModelManager* d_modelManager;

public:
    EditorUI(const Sprocket::CoreSystems& core, WorldLayer* worldLayer);

    void handleEvent(Sprocket::Event& event) override;
    void update(float dt) override;

    friend class EscapeMenu;
};