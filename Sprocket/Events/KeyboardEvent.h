#pragma once
#include "Event.h"

namespace Sprocket {

class KeyboardButtonPressedEvent : public Event
{
	int d_key;
	int d_scancode;
	int d_mods;

public:
	KeyboardButtonPressedEvent(int key, int scancode, int mods);
	virtual int categoryFlags() const override { return KEYBOARD_BUTTON | INPUT; }
	virtual const char* name() const override { return "KEYBOARD_BUTTON_PRESSED"; }
	std::string toString() const override;

	int key() const { return d_key; }
	int scancode() const { return d_scancode; }
	int mods() const { return d_mods; }
};

class KeyboardButtonReleasedEvent : public Event
{
	int d_key;
	int d_scancode;
	int d_mods;

public:
	KeyboardButtonReleasedEvent(int key, int scancode, int mods);
	virtual int categoryFlags() const override { return KEYBOARD_BUTTON; }
	virtual const char* name() const override { return "KEYBOARD_BUTTON_RELEASED"; }
	std::string toString() const override;

	int key() const { return d_key; }
	int scancode() const { return d_scancode; }
	int mods() const { return d_mods; }
};

class KeyboardButtonHeldEvent : public Event
{
	int d_key;
	int d_scancode;
	int d_mods;

public:
	KeyboardButtonHeldEvent(int key, int scancode, int mods);
	virtual int categoryFlags() const override { return KEYBOARD_BUTTON | INPUT; }
	virtual const char* name() const override { return "KEYBOARD_BUTTON_HELD"; }
	std::string toString() const override;

	int key() const { return d_key; }
	int scancode() const { return d_scancode; }
	int mods() const { return d_mods; }
};

}
