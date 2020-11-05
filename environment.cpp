#include "environment.h"

/// STRUCT ENV_OBJECT

env_object::env_object(): rect{Rectangle{0,0,100,50}}, sides{true, false, false, false}, spriteId{255}, type{BLOCK}, color{DARKGRAY}, label{"block"}, func{STATIC}
{}

env_object::env_object(Rectangle rect, unsigned short uuId): env_object()
{
    this->rect = rect;
    this->uuId = uuId;
}

void env_object::setSides(bool a, bool b, bool c, bool d)
{
	// TODO make better
	sides[0] = a;
	sides[1] = b;
	sides[2] = c;
	sides[3] = d;
}

void DrawEnvObject(const env_object o)
{
    if (o.type == BLOCK)
    {
    	// dev texture id (colored rects)
        if (o.spriteId == 255)
        {
			// COLOR CHANGE VERSION
			unsigned char cOff = 40;
			unsigned char mid = 128;
			unsigned char maxx = 255;

			Color alt;
			// COLOR AWARE CHANGING (SOME DARKER SOME LIGHTER)
			if (ENV_RENDER_TYPE == 0)
			{
				alt = Color{	(o.color.r > mid) ? (unsigned char)(o.color.r - cOff) : (unsigned char)(o.color.r + cOff),
								(o.color.g > mid) ? (unsigned char)(o.color.g - cOff) : (unsigned char)(o.color.g + cOff),
								(o.color.b > mid) ? (unsigned char)(o.color.b - cOff) : (unsigned char)(o.color.b + cOff),
								o.color.a};
			}
			else if (ENV_RENDER_TYPE == 1)
			{
			// CENTERS ARE DARKER
				alt = Color{	(o.color.r > cOff) ? (unsigned char)(o.color.r - cOff) : (unsigned char)0,
								(o.color.g > cOff) ? (unsigned char)(o.color.g - cOff) : (unsigned char)0,
								(o.color.b > cOff) ? (unsigned char)(o.color.b - cOff) : (unsigned char)0,
								o.color.a};
			}
			else
			{
			// CENTERS ARE LIGHTER
				alt = Color{	(o.color.r < maxx - cOff) ? (unsigned char)(o.color.r + cOff) : maxx,
								(o.color.g < maxx - cOff) ? (unsigned char)(o.color.g + cOff) : maxx,
								(o.color.b < maxx - cOff) ? (unsigned char)(o.color.b + cOff) : maxx,
								o.color.a};
			}

            DrawRectangleRec(o.rect, alt);

			// draw each side independently based on the value of o.sides[].
			float lineWidth = 15;
			if (o.sides[0])
			{
				DrawLineEx(Vector2{o.rect.x, o.rect.y + lineWidth/2}, Vector2{o.rect.x + o.rect.width, o.rect.y + lineWidth/2}, lineWidth, o.color);
			}
			if (o.sides[1])
			{
				DrawLineEx(Vector2{o.rect.x + o.rect.width - lineWidth/2, o.rect.y}, Vector2{o.rect.x + o.rect.width - lineWidth/2, o.rect.y + o.rect.height}, lineWidth, o.color);
			}
			if (o.sides[2])
			{
				DrawLineEx(Vector2{o.rect.x + o.rect.width, o.rect.y + o.rect.height - lineWidth/2}, Vector2{o.rect.x, o.rect.y + o.rect.height - lineWidth/2}, lineWidth, o.color);
			}
			if (o.sides[3])
			{
				DrawLineEx(Vector2{o.rect.x + lineWidth/2, o.rect.y + o.rect.height}, Vector2{o.rect.x + lineWidth/2, o.rect.y}, lineWidth, o.color);
			}
        }
    }
    else if (o.type == TEXT)
    {
        DrawTextRec(GetFontDefault(), o.label.c_str(), o.rect, 20, 1.0, true, o.color);
    }
}

void DrawEnvList(const env_list l)
{
	auto i = l.begin(), iE = l.end();
	while (i != iE)
		DrawEnvObject(*(i++));
}

/// STRUCT ENV_LEVEL

env_level::env_level(): id{(unsigned short)0}, label{"SAMPLE TEXT"}, player_start{Vector2{100,100}}, env_objects{0}
{}

env_level::env_level(unsigned short id, std::string label, env_list env_objects): id{id}, label{label}, player_start{Vector2{100,100}}, env_objects{env_objects}
{}
