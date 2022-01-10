#ifndef EDITOR_BUTTON_H_
#define EDITOR_BUTTON_H_

#include "raylib.h"

#include <functional>
#include <string>

class Button
{
public:
	Vector2 pos;
	Vector2 size;
	Rectangle rectangle;
	std::string label;
	std::function<void()> onButtonPress;

	bool isHovered;
	bool isPressed;
	Button(Rectangle rect, std::string label, std::function<void()> func);

	void Update(Vector2 mousePosition, bool mousePressed);
	void Draw(void);
	void Pressed(void);
};

#endif