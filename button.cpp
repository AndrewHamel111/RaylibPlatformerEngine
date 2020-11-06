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

bool ImageButtonSink(Rectangle bounds, Texture2D atlas, Rectangle source)
{
	Vector2 m = GetMousePosition();
	Color c = WHITE;
	bool q = false;

	if (m < bounds)
	{
		c = LIGHTGRAY;

		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
		{
			source.y -= 9;
			q = true;
		}

	}

	// draw the button
	DrawTextureRec(atlas, source, Vector2{bounds.x,bounds.y}, c);

	return q;
}

bool TextButton(Rectangle bounds, std::string text, Color buttonColor, Color textColor)
{
	Vector2 m = GetMousePosition();
	Color c = buttonColor;
	Color o = buttonColor;
	bool q = false;
	float fontSize = 20;

	if (m < bounds)
	{
		// change render color of button
		unsigned char cOff = 40;
		unsigned char mid = 128;
		unsigned char maxx = 255;

		c = Color{	(o.r > mid) ? (unsigned char)(o.r - cOff) : (unsigned char)(o.r + cOff),
					(o.g > mid) ? (unsigned char)(o.g - cOff) : (unsigned char)(o.g + cOff),
					(o.b > mid) ? (unsigned char)(o.b - cOff) : (unsigned char)(o.b + cOff),
					o.a};

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
			q = true;
	}

	// draw the button
	DrawRectangleRec(bounds, c);

	/// todo fix the alignment on this text
	DrawTextRec(GetFontDefault(), text.c_str(), bounds, fontSize, 1.0, true, textColor);

	return q;
}

bool HiddenButton(Rectangle bounds)
{
	return GetMousePosition() < bounds && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
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
int letterKeys[] = {KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z, KEY_PERIOD, KEY_MINUS} ;

bool SimpleTextBox(Rectangle r, char* c, int max_field_length)
{
	/// note that it's the caller's responsibility to ensure that c has enough room for max_field_length.
	int field_length = strlen(c);

	for(int key : letterKeys)
	{
		if (IsKeyPressed(key) && field_length < max_field_length)
		{
			c[field_length] = key;
			c[field_length + 1] = '\0';
			field_length++;
		}
	}

	if (IsKeyPressed(KEY_BACKSPACE) && field_length > 0)
	{
		c[field_length - 1] = '\0';
		field_length--;
	}
	else if (IsKeyPressed(KEY_ENTER)) return true;

	// draw button
	DrawRectangleRec(r, RAYWHITE);

	// draw TEXT
	/// TODO make fontsize grow to fit the size of the text box
	int fontSize = 20;
	DrawText(c, r.x + 10, r.y + 10, fontSize , Color{20,20,20,255});

	return false;
}
