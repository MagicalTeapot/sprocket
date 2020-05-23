#pragma once
#include "Window.h"
#include "Quad.h"
#include "WidgetProperty.h"

namespace Sprocket {

class Widget;

class VerticalConstraint : public WidgetProperty
{
public:
    enum class Type
    {
        TOP,
        CENTRE,
        BOTTOM
    };

private:
    Type  d_type;

    float d_offset;
        // Offset from the side in pixels. This is ignored for
        // CENTRE constraints. TODO: Can we do better than this?

public:
    VerticalConstraint(Type type, float offset);

    virtual void update(Widget* widget, Window* window) override;

    virtual void handleEvent(Widget* widget,
                             Window* window,
                             Event& event) override {}
};

}