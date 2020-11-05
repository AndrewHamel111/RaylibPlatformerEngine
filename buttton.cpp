#include "button.h"

bool ImageButtonSpriteSwap(Rectangle bounds, Texture2D atlas, Rectangle source, Rectangle altSource)
{
	Vector2 m = GetMousePosition();
	Color c = WHITE;
	bool q = false;

	Rectangle src = source;

	if (m < bounds)
	{
		// change atlas source for button
		src = altSource;

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			c = GRAY;
			q = true;
		}
	}

	// draw the button
	DrawTextureRec(atlas, src, Vector2{bounds.x,bounds.y}, c);

	return q;
}

bool ImageButtonEx(Rectangle bounds, Texture2D atlas, Rectangle source)
{
	Vector2 m = GetMousePosition();
	Color c = WHITE;
	bool q = false;

	if (m < bounds)
	{
		// change render color of button
		c = LIGHTGRAY;

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			c = GRAY;
			q = true;
		}
	}

	// draw the button
	DrawTextureRec(atlas, source, Vector2{bounds.x,bounds.y}, c);

	return q;
}

bool HiddenButton(Rectangle bounds)
{
	return Vector2{GetMouseX(), GetMouseY()} < bounds && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

float SliderBar(Vector2 start, float length, float* percent, float scale)
{
	bool isHovering = GetMousePosition() < Rectangle{start.x - 20, start.y - 30, length + 40, 60};
	Color col;

	  ////////////
	//// UPDATE ////
	  ////////////

	if (isHovering && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
	{
		col = DARKBLUE;

		// use mousepos to find the new percent
		*percent = ((GetMouseX() - start.x) / length);

		// clamp percent to [0,1]
		if (*percent < 0) *percent = 0;
		else if (*percent > 1.0) *percent = 1.0;
	}
	else if (isHovering)
		col = BLUE;
	else
		col = WHITE;

	  //////////
	//// DRAW ////
	  //////////

	Vector2 handlePosition = start;
	// offset
	handlePosition.x -= SLIDER_HANDLE_HEIGHT/2;
	handlePosition.y -= SLIDER_HANDLE_HEIGHT/2;

	// adjust X pos to the current percentage
	handlePosition.x += (*percent) * length;

	// draw bar and handle
	DrawRectangleRec(Rectangle{start.x, start.y - SLIDER_HEIGHT/2, length, SLIDER_HEIGHT}, GRAY);
	DrawRectangleRec(Rectangle{handlePosition.x, handlePosition.y, SLIDER_HANDLE_HEIGHT, SLIDER_HANDLE_HEIGHT}, col);

	return (*percent) * scale;
}

float SliderBarCenter(Vector2 center, float length, float* percent, float scale)
{
	Vector2 v = center;
	v.x -= length/2;
	v.y -= SLIDER_HEIGHT/2;

	return SliderBar(v, length, percent, scale);
}
