#include "editor_button.h"
#include "button.h"

Button::Button(Rectangle rect, std::string label, std::function<void()> func) : rectangle{rect}, label{label}, onButtonPress{func}
{
	this->pos = Vector2 {
		rectangle.x + rectangle.width/2,
		rectangle.y + rectangle.height/2
	};
	this->size = Vector2 {
		rectangle.width,
		rectangle.height
	};
}

void Button::Update(Vector2 mousePosition, bool mousePressed)
{
	isHovered = isPressed = false;

	if (CheckCollisionPointRec(mousePosition, rectangle))
	{
		isHovered = true;

		if (mousePressed)
			Pressed();
	}
}

void Button::Draw(void)
{
	DrawEditorButton(*this);
}

void Button::Pressed(void)
{
	isPressed = true;
	this->onButtonPress();
}