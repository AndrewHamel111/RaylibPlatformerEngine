#ifndef PLAYER_H_
#define PLAYER_H_

/*
IDEAS FOR PLAYER MOVEMENT

- use the principle as outlined in sm64, i.e. updates occur in the following order:
	- move the player (allow clipping)
	- the walls/floors push the player back into the playable area
	
	- draw the player some time after all these have been resolved
- player can wall jump on the same frame that they "entered" a wall

*/

#define PLAYER_FLAG_COUNT 6

#define PLAYER_BASE_ACCELERATION 1.5
#define PLAYER_DECELERATION 0.9
#define PLAYER_MAX_VELOCITY 10
#define PLAYER_GRAVITY 1
#define PLAYER_JUMP_VELOCITY 15

#ifndef RAYLIB_H_
	#define RAYLIB_H_
	#include "raylib.h"
#endif

#include "constants.h"
#include "environment.h"
#include "operators.h"

#include <cmath>
#include <string>

#ifndef NEARBLACK
	#define NEARBLACK CLITERAL(Color){ 20, 20, 20, 255}
#endif

class player
{
	// FIELDS
	
	Vector2 pos; /**< Position of the player. */
	Vector2 vel; /**< Current velocity of the player.*/
	Vector2 acc; /**< Acceleration of the player. This may be affected by powerups, holding shift, etc.*/
	/// Generally pos += vel is unbounded, vel += acc is bounded.
	
	// player flags and aux values (state values)
	bool _flag[PLAYER_FLAG_COUNT] = {false};
	float _value[PLAYER_FLAG_COUNT] = {0};
	
	
	// METHODS
	
	
	
	// OTHER
	
	enum PLAYER_FLAG { IN_WALL_LEFT, IN_WALL_RIGHT, IN_CEILING, IN_FLOOR, IS_JUMPING, ON_GROUND};
	
	
public:
	
	/// CONSTRUCTORS
	
	player();  /**< Default constructor initializes player to the center of the screen with no velocity and default acc. */
	player(Vector2);  /**< Position constructor only specifies the starting position of the player. */
	
	/// ACCESS METHODS
	
	Vector2 getPos() const;
	Vector2 getVel() const;
	Vector2 getAcc() const;
	
	
	/// UPDATE AND DRAWING METHODS
	
	void move();  /**< apply any motions from input (clip into walls) */
	void check(env_list*);  /**< check for any collisions and adjust player's flags accordingly. */
	void correct();	/**< use flags from check to correct the player's state */
	
	void update(env_list*); /**< update method for the player simply calls the above methods in the correct order. Distributes the parameters out accordingly. */
	
	void DrawPlayer();
};

#endif