// TODO
/*
** - level geometry features
** - keys and doors, where the nth key in env_list corresponds to the nth door in env_list
** - alt color assignment in DrawEnvObject needs to be hidden in a function or two. I think hamlib already has this?
** - we can't be drawing EVERYTHING in the environment, same goes for collision, so decide on a simple way to skip certain draws / updates and we can iterate on it further at a later time
*/

#include "environment.h"

/// STRUCT ENV_OBJECT

env_object::env_object(): rect{Rectangle{0,0,100,50}}, sides{true, false, false, false}, spriteId{255}, type{BLOCK}, color{DARKGRAY}, label{"block"}, isCollected{false}, boostAcc{Vector2{0,0}}, boostMag{1.0f}, func{STATIC}
{}

env_object::env_object(Rectangle rect): env_object()
{
	this->rect = rect;
}

env_object::env_object(Rectangle rect, unsigned short uuId): env_object()
{
	this->rect = rect;
	this->uuId = uuId;
}

env_object::env_object(Rectangle rect, ENV_OBJECT_TYPE type): env_object()
{
	switch(type)
	{
	case BLOCK:
		break;
	case TEXT:
		break;
	case COIN:
		color = Color{245, 224, 66, 200};
		break;
	}
}

void env_object::setSides(bool a, bool b, bool c, bool d)
{
	// TODO make better
	sides[0] = a;
	sides[1] = b;
	sides[2] = c;
	sides[3] = d;

	boostAcc.x = boostAcc.y = 0;

	if (type == BOOST)
	{
		if (sides[0])
			boostAcc.y -= BOOST_BASE_VALUE;
		else if (sides[2])
			boostAcc.y += BOOST_BASE_VALUE;
		if (sides[1])
			boostAcc.x += BOOST_BASE_VALUE;
		else if (sides[3])
			boostAcc.x -= BOOST_BASE_VALUE;
	}
}

/* Original as gone unchanged. Not to be used. */
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
	else if (o.type == COIN || o.type == HAZARD)
	{
		DrawRectangleRec(o.rect, o.color);
	}
	else if (o.type == BOOST)
	{
		// DEV SHIT. temp semitrans rectangle for boost and some arrows
		DrawRectangleRec(o.rect, Color{100,100,100,80});

		// arrows
		if (o.sides[0])
			DrawArrow(Vector2{o.rect.x + o.rect.width/2, o.rect.y + 5}, 30, 0);
		if (o.sides[1])
			DrawArrow(Vector2{o.rect.x + o.rect.width - 5, o.rect.y + o.rect.height/2}, 30, 1);
		if (o.sides[2])
			DrawArrow(Vector2{o.rect.x + o.rect.width/2, o.rect.y + o.rect.height - 5}, 30, 2);
		if (o.sides[3])
			DrawArrow(Vector2{o.rect.x + 5, o.rect.y + o.rect.height/2}, 30, 3);
	}
	else if (o.type == LAUNCH)
	{
		// DEV SHIT. temp semitrans rectangle for boost and some arrows
		Color col = Color{100,250,100,80};
		DrawRectangleRec(o.rect, col);
		col.a = 120;

		// arrows
		if (o.sides[0])
			DrawArrow(Vector2{o.rect.x + o.rect.width/2, o.rect.y + 5}, 30, 0, col);
		if (o.sides[1])
			DrawArrow(Vector2{o.rect.x + o.rect.width - 5, o.rect.y + o.rect.height/2}, 30, 1, col);
		if (o.sides[2])
			DrawArrow(Vector2{o.rect.x + o.rect.width/2, o.rect.y + o.rect.height - 5}, 30, 2, col);
		if (o.sides[3])
			DrawArrow(Vector2{o.rect.x + 5, o.rect.y + o.rect.height/2}, 30, 3, col);
	}
}

/* Only for foreground objects (objects that stop the player or are collected by him) */
void DrawFGEnvObject(const env_object o, Texture2D atlas, RectSet rects)
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
			// TODO review this
			if (o.rect.height < lineWidth)
				lineWidth = o.rect.height/2;
			if (o.rect.width < lineWidth)
				lineWidth = o.rect.width/2;

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
	else if (o.type == COIN || o.type == HAZARD)
	{
		DrawRectangleRec(o.rect, o.color);
	}
	else if (o.type == BOOST)
	{
		// DEV SHIT. temp semitrans rectangle for boost and some arrows
		DrawRectangleRec(o.rect, Color{100,100,100,80});

		// arrows
		if (o.sides[0])
			DrawArrow(Vector2{o.rect.x + o.rect.width/2, o.rect.y + 5}, 30, 0);
		if (o.sides[1])
			DrawArrow(Vector2{o.rect.x + o.rect.width - 5, o.rect.y + o.rect.height/2}, 30, 1);
		if (o.sides[2])
			DrawArrow(Vector2{o.rect.x + o.rect.width/2, o.rect.y + o.rect.height - 5}, 30, 2);
		if (o.sides[3])
			DrawArrow(Vector2{o.rect.x + 5, o.rect.y + o.rect.height/2}, 30, 3);
	}
	else if (o.type == LAUNCH)
	{
		// DEV SHIT. temp semitrans rectangle for boost and some arrows
		Color col = Color{100,250,100,80};
		DrawRectangleRec(o.rect, col);
		col.a = 120;

		// arrows
		if (o.sides[0])
			DrawArrow(Vector2{o.rect.x + o.rect.width/2, o.rect.y + 5}, 30, 0, col);
		if (o.sides[1])
			DrawArrow(Vector2{o.rect.x + o.rect.width - 5, o.rect.y + o.rect.height/2}, 30, 1, col);
		if (o.sides[2])
			DrawArrow(Vector2{o.rect.x + o.rect.width/2, o.rect.y + o.rect.height - 5}, 30, 2, col);
		if (o.sides[3])
			DrawArrow(Vector2{o.rect.x + 5, o.rect.y + o.rect.height/2}, 30, 3, col);
	}
}

/* Only for background objects (objects that don't stop the player but still matter :) */
void DrawBGEnvObject(const env_object o, Texture2D atlas, RectSet rects)
{
	if (o.type == TEXT)
	{
		//DrawTextRec(GetFontDefault(), o.label.c_str(), o.rect, 20, 1.0, true, o.color);
		DrawText(o.label.c_str(), o.rect.x, o.rect.y, 20, o.color);
	}
	else if (o.type == GOAL)
	{
		Vector2 zero = Vector2{0,0};
		DrawTexturePro(atlas, rects["flag"], o.rect, zero, 0.0f, WHITE);
	}
}

void DrawArrow(Vector2 point, float length, int dir, Color col) // up, right, down, left
{
	//Color col{100,100,100,200};

	Vector2 p2;
	Vector2 p3;
	Rectangle rect;

	float triangleL = length/3;	// long length of triangle
	float rectL = 2*length/3;		// long length of rectangle
	float triangleS = 20;		// short length of triangle
	float rectS = triangleS/3;	// short length of rectangle

	if (dir == 0)
	{
		p2 = Vector2{point.x - triangleS/2, point.y + triangleL};
		p3 = Vector2{point.x + triangleS/2, point.y + triangleL};
		rect = Rectangle{point.x - rectS/2, point.y + triangleL, rectS, rectL};
	}
	else if (dir == 1)
	{
		p2 = Vector2{point.x - triangleL, point.y - triangleS/2};
		p3 = Vector2{point.x - triangleL, point.y + triangleS/2};
		rect = Rectangle{point.x - triangleL - rectL, point.y - rectS/2, rectL, rectS};
	}
	else if (dir == 2)
	{
		p2 = Vector2{point.x + triangleS/2, point.y - triangleL};
		p3 = Vector2{point.x - triangleS/2, point.y - triangleL};
		rect = Rectangle{point.x - rectS/2, point.y - triangleL - rectL, rectS, rectL};
	}
	else // if (dir == 3)
	{
		p2 = Vector2{point.x + triangleL, point.y + triangleS/2};
		p3 = Vector2{point.x + triangleL, point.y - triangleS/2};
		rect = Rectangle{point.x + triangleL, point.y - rectS/2, rectL, rectS};
	}

	DrawTriangle(point, p2, p3, col);
	DrawRectangleRec(rect, col);
}

void DrawLevel(env_level level)
{
	auto l = level.env_objects;
	auto i = l.begin(), iE = l.end();

	while (i != iE)
	{
		if (i->type == GOAL || i->type == TEXT)
			DrawBGEnvObject(*i, level.backgroundAtlas, level.backgroundRect);
		else
			DrawFGEnvObject(*i, level.foregroundAtlas, level.foregroundRect);

		i++;
	}
}

bool operator<(const env_level& a, const env_level& b)
{
	return a.id < b.id;
}

/// STRUCT ENV_LEVEL

env_level::env_level(): id{(unsigned short)0}, label{"SAMPLE TEXT"}, player_start{Vector2{100,100}}, env_objects{0}
{}

env_level::env_level(unsigned short id, std::string label, env_list env_objects): id{id}, label{label}, player_start{Vector2{100,100}}, env_objects{env_objects}
{}

Color RandomColor()
{
	return Color{(unsigned char)(rand() % 255), (unsigned char)(rand() % 255), (unsigned char)(rand() % 255), 255};
}
