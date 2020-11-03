#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include "raylib.h"

#include <vector>
#include <string>

enum ENV_OBJECT_TYPE {
    BLOCK, TEXT
};

enum ENV_OBJECT_FUNCTION {
    STATIC, DYNAMIC
};

/**
	A basic struct representing an object in the environment. This can take the form of a floor, wall, ceiling, platform, etc.
*/
struct env_object {
	Rectangle rect; /**< The rayib::Rectangle acting as the bounds of the hitbox, as well as the dest_rect for when the object is drawn. */
	bool sides[4]; /**< An array describing the solid properties of the Rect. The default choice is the mario "semi-solid", where the player can jump up from the bottom and land on the top but run through the sides freely. */
	unsigned short id; /**< id is used as an index to a set of sprite rects: it is used to determine how the object should be displayed. */
	ENV_OBJECT_TYPE type;
	ENV_OBJECT_FUNCTION func;
	Color color;
	std::string label;

	env_object();
	env_object(Rectangle rect);
};

/** A simple alias to help reduce the number of times I need to use my :: With my new keyboard I often type M< instead of <> */
using env_list = std::vector<env_object>;

/**
	A basic struct representing an environment level. It consists of a uid for the level and a collection of objects.
*/
struct env_level {
	unsigned short id; /**< unique level identifier */
	std::string label; /**< label of the level */
	env_list env_objects; /**< std::vector<env_object> of all the environment objects */

	env_level();
	env_level(unsigned short, std::string, env_list);
};

void DrawEnvObject(const env_object);
void DrawEnvList(const env_list);


#endif
