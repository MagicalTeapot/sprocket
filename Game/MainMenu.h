#pragma once
#include <Sprocket.h>

class MainMenu : public Sprocket::Layer
{
    Sprocket::Container       d_container;
    Sprocket::DisplayRenderer d_renderer;

public:
    MainMenu(const Sprocket::CoreSystems& core);

    bool handleEventImpl(const Sprocket::Event& event) override;
    void updateImpl() override;
    void drawImpl() override;
};