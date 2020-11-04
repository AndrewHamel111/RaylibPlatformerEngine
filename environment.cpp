#include "environment.h"

/// STRUCT ENV_OBJECT

env_object::env_object(): rect{Rectangle{0,0,100,50}}, sides{true, false, false, false}, id{255}, type{BLOCK}, color{DARKGRAY}, label{""}
{}

env_object::env_object(Rectangle rect): env_object()
{
    this->rect = rect;
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
        if (o.id == 255)
        {
        	// draw slightly "lighter" color for the center of the rectangle
			//Rectangle center{o.rect.x + 10, o.rect.y + 10, o.rect.width - 20, o.rect.height - 20};
			// ALPHA VERSION
			//Color light{o.color.r, o.color.g, o.color.b, (int)(o.color.a * 0.9f)};
			// COLOR CHANGE VERSION
			const unsigned int cOff = 20;
			/*
			// COLOR AWARE CHANGING (SOME DARKER SOME LIGHTER)
			Color light{(o.color.r > 128) ? o.color.r - cOff : o.color.r + cOff,
						(o.color.g > 128) ? o.color.g - cOff : o.color.g + cOff,
						(o.color.b > 128) ? o.color.b - cOff : o.color.b + cOff,
						o.color.a};
						*/
						/*
						// CENTERS ARE DARKER
			Color light{(o.color.r > cOff) ? o.color.r - cOff : 0,
						(o.color.g > cOff) ? o.color.g - cOff : 0,
						(o.color.b > cOff) ? o.color.b - cOff : 0,
						o.color.a};
						*/
						// CENTERS ARE LIGHTER
			Color light{(o.color.r < 255 - cOff) ? o.color.r + cOff : 255,
						(o.color.g < 255 - cOff) ? o.color.g + cOff : 255,
						(o.color.b < 255 - cOff) ? o.color.b + cOff : 255,
						o.color.a};

            DrawRectangleRec(o.rect, light);

			// draw each side independently based on the value of o.sides[].
			Color col;
			if (o.sides[0])
			{
				DrawLineEx(Vector2{o.rect.x, o.rect.y + 5}, Vector2{o.rect.x + o.rect.width, o.rect.y + 5}, 10, o.color);
			}
			if (o.sides[1])
			{
				DrawLineEx(Vector2{o.rect.x + o.rect.width - 5, o.rect.y}, Vector2{o.rect.x + o.rect.width - 5, o.rect.y + o.rect.height}, 10, o.color);
			}
			if (o.sides[2])
			{
				DrawLineEx(Vector2{o.rect.x + o.rect.width, o.rect.y + o.rect.height - 5}, Vector2{o.rect.x, o.rect.y + o.rect.height - 5}, 10, o.color);
			}
			if (o.sides[3])
			{
				DrawLineEx(Vector2{o.rect.x + 5, o.rect.y + o.rect.height}, Vector2{o.rect.x + 5, o.rect.y}, 10, o.color);
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

env_level::env_level(): id{0}, label{"SAMPLE TEXT"}, env_objects{0}
{}

env_level::env_level(unsigned short id, std::string label, env_list env_objects): id{id}, label{label}, env_objects{env_objects}
{}
