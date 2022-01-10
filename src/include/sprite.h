#ifndef SPRITE_H_
#define SPRITE_H_

#include "raylib.h"

struct Sprite
{
	Texture2D atlas;
	Rectangle source;

	int width;
	int height;

	Sprite(Texture2D atlas, Rectangle source, int width, int height)
	{
		this->atlas = atlas;
		this->source = source;
		this->width = width;
		this->height = height;
	};

	Sprite(Texture2D atlas, Rectangle source): width{source.width}, height{source.height}
	{
		this->atlas = atlas;
		this->source = source;
	}

	Sprite(Texture2D atlas): source{0,0,atlas.width,atlas.height}, width{source.width}, height{source.height}
	{
		this->atlas = atlas;
	}
};

#endif