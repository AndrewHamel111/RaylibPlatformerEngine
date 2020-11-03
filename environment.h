#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include "raylib.h"

#include <vector>

/**
 A basic struct representing an object in the environment. This can take the form of a floor, wall, ceiling, platform, etc.
*/
struct env_object {
	Rectangle rect; /**< The rayib::Rectangle acting as the bounds of the hitbox, as well as the dest_rect for when the object is drawn. */
	bool sides[4]; /**< An array describing the solid properties of the Rect. The default choice is the mario "semi-solid", where the player can jump up from the bottom and land on the top but run through the sides freely. */
	unsigned short id; /**< id is used as an index to a set of sprite rects: it is used to determine how the object should be displayed. */
	Color color;
	
	env_object();
	env_object(Rectangle rect);
};

void DrawEnvObject(const env_object);
void DrawEnvList(const env_list);

/** A simple alias to help reduce the number of times I need to use my :: With my new keyboard I often type M< instead of <> */
using env_list = std::vector<env_object>;

#endif