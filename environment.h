#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#define ENV_RENDER_TYPE 2
#define BOOST_BASE_VALUE 3
#define LAUNCH_BASE_VALUE 20

#include "raylib.h"
#include "animation.h"

#include <vector>
#include <string>

enum ENV_OBJECT_TYPE
{
	BLOCK, TEXT, COIN, BOOST, LAUNCH
};

enum ENV_OBJECT_FUNCTION
{
	STATIC, DYNAMIC
};

/**
	A basic struct representing an object in the environment. This can take the form of a floor, wall, ceiling, platform, etc.
*/
struct env_object
{
	Rectangle rect; /**< The rayib::Rectangle acting as the bounds of the hitbox, as well as the dest_rect for when the object is drawn. */
	bool sides[4]; /**< An array describing the solid properties of the Rect. The default choice is the mario "semi-solid", where the player can jump up from the bottom and land on the top but run through the sides freely. */
	unsigned short spriteId; /**< id is used as an index to a set of sprite rects: it is used to determine how the object should be displayed. */
	unsigned short uuId; /**< a unique id issued by the env_level to have a unique identifier for each object in the level. env_object::uuId is unique only in it's own env_list. */
	ENV_OBJECT_TYPE type;
	ENV_OBJECT_FUNCTION func;
	Color color;
	std::string label;

	bool isCollected; /**< only for use by COIN */
	/// might be deprecated
	Vector2 boostAcc; /**< only for use by BOOST and LAUNCH */
	float boostMag; /**< only for use by BOOST and LAUNCH */

	env_object();
	env_object(Rectangle);
	env_object(Rectangle, unsigned short);
	env_object(Rectangle, ENV_OBJECT_TYPE);

	void setSides(bool, bool, bool, bool);
};

/** A simple alias to help reduce the number of times I need to use my :: With my new keyboard I often type M< instead of <> */
using env_list = std::vector<env_object>;

/**
	A basic struct representing an environment level. It consists of a uid for the level and a collection of objects.
*/
struct env_level
{
	unsigned short id; /**< unique level identifier */
	std::string label; /**< label of the level */
	Vector2 player_start; /**< starting position of player */
	env_list env_objects; /**< std::vector<env_object> of all the environment objects */

	env_level();
	env_level(unsigned short, std::string, env_list);
};

void DrawEnvObject(const env_object);
void DrawArrow(Vector2, float, int, Color col = Color{100,100,100,200});
void DrawEnvList(const env_list);

Color RandomColor();

bool operator<(const env_level& a, const env_level& b);

#endif
