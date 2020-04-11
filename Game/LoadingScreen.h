#pragma once
#include <Sprocket.h>

class LoadingScreen : public Sprocket::Layer
{
    Sprocket::Container       d_container;
    Sprocket::DisplayRenderer d_renderer;

public:
    LoadingScreen(Sprocket::Accessor& accessor);

    bool handleEventImpl(const Sprocket::Event& event) override;
    void updateImpl() override;
    void drawImpl() override;
};