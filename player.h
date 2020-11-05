#ifndef PLAYER_H_
#define PLAYER_H_

//{ DEFINE
#define PLAYER_FLAG_COUNT 7

/// speed values (normal)
#define PLAYER_BASE_ACCELERATION 1.5
#define PLAYER_DECELERATION 0.9
#define PLAYER_MAX_VELOCITY 10

/// speed values (sprinting)
#define PLAYER_SPRINT_ACCELERATION 2
#define PLAYER_SPRINT_DECELERATION 1.15
#define PLAYER_SPRINT_VELOCITY 13
#define PLAYER_WALL_JUMP_SPRINT 1.3

/// gravity and jump
#define PLAYER_GRAVITY 1
#define PLAYER_JUMP_VELOCITY 15
#define PLAYER_WALL_JUMP_MODIFIER 0.95

/// other
#define PLAYER_HITBOX_PERCENTAGE 0.15

#ifndef RAYLIB_H_
	#define RAYLIB_H_
	#include "raylib.h"
#endif

#include "constants.h"
#include "environment.h"
#include "operators.h"

#include <cmath>
#include <string>

// TEMPORARY, FOR DEBUGGING ONLY
#include <iostream>

#ifndef NEARBLACK
	#define NEARBLACK CLITERAL(Color){ 20, 20, 20, 255}
#endif

//}

enum LineCheckDirection {
	UP, RIGHT, LEFT, DOWN
};

class player
{
	// FIELDS

	// player flags and aux values (state values)
	bool _flag[PLAYER_FLAG_COUNT] = {false};
	float _value[PLAYER_FLAG_COUNT] = {0};

	// TODO fuck this
	// write a method that checks the perimeter of the hitbox one side at a time for collisions
	// the method takes a parameter that determines how many checks it should perform to form a line
	// i.e. lineCheck(10) checks 10 places equally spaced between the corners (these checks should occur
	// using the same bounds as the default hitbox_anchors, provided here for reference.
	/* (this is a representation in a more graphical fashion of the corners)
	Vector2{-51, -101}		Vector2{0, -101} 	Vector2{51, -101}
	Vector2{-51, -50}							Vector2{51, -50}
	Vector2{-51, 1}			Vector2{0, 1}		Vector2{51, 1}
	*/
	std::vector<Vector2> hitbox_anchors; /**< std::vector of anchors to check collisions with. */
	Vector2 hitbox_corners[4];

	// METHODS


	// OTHER

	enum PLAYER_FLAG { ON_WALL_LEFT, ON_WALL_RIGHT, IN_CEILING, IN_FLOOR, IS_JUMPING, ON_GROUND, HOLDING_JUMP};


public:
	Vector2 pos; /**< Position of the player. pos = (x,y) where x should be the horizontal center and y is the lowest point (feet) */
	Vector2 vel; /**< Current velocity of the player.*/
	Vector2 acc; /**< Acceleration of the player. This may be affected by powerups, holding shift, etc.*/

	Vector2 hitboxSize; /**< Width and height of the player's hitbox. */

                            /// CONSTRUCTORS
	player();  /**< Default constructor initializes player to the center of the screen with no velocity and default acc. */
	player(Vector2);  /**< Position constructor only specifies the starting position of the player. */

                            /// ACCESS METHODS
	/// DEPRECATED, SINCE POS, VEL, ACC ARE NOW PUBLIC
	Vector2 getPos() const;
	Vector2 getVel() const;
	Vector2 getAcc() const;

	void setPos(Vector2);
	void setPos(float, float);

	void setHitboxAnchors(std::vector<Vector2>);
	bool LineCheck(LineCheckDirection, int, env_object);

                            /// UPDATE AND DRAWING METHODS
	void move();  /**< apply any motions from input (clip into walls) */
	void check(env_list);  /**< check for any collisions and adjust player's flags accordingly. */
	void correct();	/**< use flags from check to correct the player's state */

	void update(env_list); /**< update method for the player simply calls the above methods in the correct order. Distributes the parameters out accordingly. */

	void DrawPlayer();
};

#endif
