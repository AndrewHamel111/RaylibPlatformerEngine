// TODO consider replacing the LineCheck collision system with some kind of overlap box system, that way you can figure out the relative position of the other object. In addition, perhaps the process of determining stairs / slopes can be as simple as getting the height of the overlap and checking it against some value, and then correcting for the step can be as simple as adding said height to player pos.

#include "player.h"
#include <algorithm>

// DEBUGGING ONLY
#include <iostream>

extern void ResetCurrentLevel();
extern env_level current_level;

static bool OverlapBox(Rectangle a, Rectangle b, Rectangle* out);
#define MIN(A,B) (((A) < (B)) ? (A) : (B))

#define PLAYER_STAIR_HEIGHT 10
#define PLAYER_STAIR_PENALTY 0.95f

/// CONSTRUCTORS

player::player(): pos{Vector2{400,300}}, vel{Vector2{0,0}}, acc{Vector2{0,0}}, externAcc{Vector2{0,0}}, externVel{Vector2{0,0}}, coins{0}, color{RandomColor()}, isDead{false}, godmode{false}, inEndZone{false}
{
	// DEFAULT CONSTRUCTOR

	// ADD CORNERS AND EDGES TO ANCHORS
	// top left, top right, bottom right, bottom left
	setHitboxSize(Vector2{100,100});
}

player::player(Vector2 pos): player()
{
	this->pos = pos;
	// POSITION CONSTRUCTOR
}

/// ACCESS METHODS

Vector2 player::getPos() const
{
	return pos;
}

Vector2 player::getVel() const
{
	return vel;
}

Vector2 player::getAcc() const
{
	return acc;
}

/// UPDATE AND DRAW METHODS

void player::move()
{
	if (_flag[HOLDING_JUMP] && _flag[IS_JUMPING])
		acc.y = PLAYER_GRAVITY * 0.7;
	else if (_flag[IS_JUMPING])
		acc.y = PLAYER_GRAVITY * 1.2;
	else
		acc.y = PLAYER_GRAVITY;

	float maxVel;
	float decel;
	float accel;
	float wall_jump;
	if(IsKeyDown(KEY_LEFT_SHIFT))
	{
		maxVel = PLAYER_SPRINT_VELOCITY;
		decel = PLAYER_SPRINT_DECELERATION;
		accel = PLAYER_SPRINT_ACCELERATION;
		wall_jump = PLAYER_WALL_JUMP_SPRINT;
	}
	else
	{
		maxVel = PLAYER_MAX_VELOCITY;
		decel = PLAYER_DECELERATION;
		accel = PLAYER_BASE_ACCELERATION;
		wall_jump = PLAYER_WALL_JUMP_MODIFIER;
	}

	acc.x = accel * (IsKeyDown(KEY_D) - IsKeyDown(KEY_A));

	// apply acc (when applicable)
	// TODO FIX THIS CUM
	if (abs(vel.x + acc.x ) < maxVel)
		vel.x += acc.x;

	if (vel.y + acc.y < PLAYER_TERMINAL_VELOCITY)
		vel.y += acc.y;

	// apply externAcc and externVel after the max checks
	// note that externVel basically overrides externAcc
	if (IsKeyDown(KEY_SPACE))
	{
		vel.x += externAcc.x * BOOST_JUMP_BONUS;
		vel.y += externAcc.y * BOOST_JUMP_BONUS;
	}
	else
	{
		vel.x += externAcc.x;
		vel.y += externAcc.y;
	}

	if (externVel.x != 0)
		vel.x = externVel.x;
	if (externVel.y != 0)
		vel.y = externVel.y;

	// add jump (when applicable)
	if (IsKeyPressed(KEY_SPACE) && !_flag[IS_JUMPING])
	{
		// normal jump
		if (_flag[ON_GROUND])
		{
			vel.y = (-1) * PLAYER_JUMP_VELOCITY;
			_flag[IS_JUMPING] = true;
			_flag[HOLDING_JUMP] = true;
		}
		// wall jump (from left wall)
		else if (_flag[ON_WALL_LEFT])
		{
			vel.y = (-1) * PLAYER_JUMP_VELOCITY;
			vel.x =  wall_jump * PLAYER_JUMP_VELOCITY;
			_flag[IS_JUMPING] = true;
			_flag[HOLDING_JUMP] = true;
		}
		// wall jump (from right wall)
		else if (_flag[ON_WALL_RIGHT])
		{
			vel.y = (-1) * PLAYER_JUMP_VELOCITY;
			vel.x = (-1) * wall_jump * PLAYER_JUMP_VELOCITY;
			_flag[IS_JUMPING] = true;
			_flag[HOLDING_JUMP] = true;
		}
	}
	else if (!IsKeyPressed(KEY_SPACE))
	{
		if (_flag[ON_WALL_LEFT] || _flag[ON_WALL_RIGHT])
		{
			// if the user hangs too long they will lose the ability until they separate from the wall
			if (_flag[ON_WALL_LEFT])
				_value[ON_WALL_LEFT]++;
			else if (_flag[ON_WALL_RIGHT])
				_value[ON_WALL_RIGHT]++;

			// slow fall speed drastically to allow for time to walljump
			// left wall requires valid left wall hangtime, right wall requires "" ""
			if ((_flag[ON_WALL_LEFT] && _value[ON_WALL_LEFT] < PLAYER_WALL_HANG_TIME * TARGET_FPS) || (_flag[ON_WALL_RIGHT] && _value[ON_WALL_RIGHT] < PLAYER_WALL_HANG_TIME * TARGET_FPS))
			{
				vel.y = std::min(vel.y, PLAYER_WALL_SLIDE_SPEED);
			}
		}
	}

	// cancel jump hold if we let go
	if (_flag[HOLDING_JUMP] && _flag[IS_JUMPING] && !IsKeyDown(KEY_SPACE))
		_flag[HOLDING_JUMP] = false;

	//////////////////
	//  UPDATE POSITION
	pos = pos + vel;
	//////////////////

	// apply dec
	if (vel.x != 0)
	{
		if (!_flag[ON_GROUND]) decel *= 0.5;

		if (vel.x > 0)
		{
			vel.x = (abs(vel.x) > decel) ? vel.x - decel : 0;
		}
		else
		{
			vel.x = (abs(vel.x) > decel) ? vel.x + decel : 0;
		}
	}

	// decelerate externAcc
	externAcc.x = externAcc.y = 0;
	externVel.x = externVel.y = 0;
	/// THIS IS CONSQUENCE! WHY! GO TO COLLEGE, FUCK!
	/// this is dumb stupid code and more importantly dumb stupid code that DOES NOT WORK.
	/*
	float d = PLAYER_DECELERATION / 3;
	if (externAcc.x != 0)
	{
		if (externAcc.x > 0)
			externAcc.x = (abs(externAcc.x) > d) ? externAcc.x - d : 0;
		else
			externAcc.x = (abs(externAcc.x) > d) ? externAcc.x + d : 0;
	}

	if (externAcc.y != 0)
	{
		if (externAcc.y > 0)
			externAcc.y = (abs(externAcc.y) > d) ? externAcc.y - d : 0;
		else
			externAcc.y = (abs(externAcc.y) > d) ? externAcc.y + d : 0;
	}
	*/

}/// end player::move

void player::check(env_list* env)
{
	bool temp_flag_ON_GROUND = false;
	bool temp_flag_ON_WALL_LEFT = false;
	bool temp_flag_ON_WALL_RIGHT = false;

	const int lineSegmentCount = 10;

	Rectangle playerRect = {this->pos.x - this->hitboxSize.x/2, this->pos.y - this->hitboxSize.y,
		this->hitboxSize.x, this->hitboxSize.y};

	// for each env_object
	for (auto envI = env->begin(); envI != env->end(); envI++)
	{
		Rectangle overlap;

		/// IGNORE COLLISION FOR TEXT
		if (envI->type == TEXT)
		{
			continue;
		}
		/// COLLECTION LOGIC FOR COIN
		else if (envI->type == COIN)
		{
			if (coinDist(*this, envI->rect))
			{
				envI->isCollected = true;
				this->coins++;
			}
			continue;
		}
		/// BOOST LOGIC FOR BOOST OBJ
		else if (envI->type == BOOST)
		{
			// we use an additive method so that if the player is influenced by a sideways and an upwards boost they can both be applied
			Vector2 center{this->pos.x, this->pos.y - this->hitboxSize.y/2};
			if (center < envI->rect)
			{
				if (envI->sides[0])// && this->externAcc.y > (-1) * BOOST_MAX_ACCEL)
					this->externAcc.y -= BOOST_BASE_VALUE;
				else if (envI->sides[2])// && this->externAcc.y < BOOST_MAX_ACCEL)
					this->externAcc.y += BOOST_BASE_VALUE;

				if (envI->sides[1])// && this->externAcc.y < BOOST_MAX_ACCEL)
					this->externAcc.x += BOOST_BASE_VALUE;
				else if (envI->sides[3])// && this->externAcc.y > (-1) * BOOST_MAX_ACCEL)
					this->externAcc.x -= BOOST_BASE_VALUE;
			}
			// TODO add a player flag to indicate the player is being boosted (unique animation perhaps)

			continue;
		}
		/// LAUNCH LOGIC FOR LAUNCH OBJ
		else if (envI->type == LAUNCH)
		{
			// we use an additive method so that if the player is influenced by a sideways and an upwards boost they can both be applied
			Vector2 center{this->pos.x, this->pos.y - this->hitboxSize.y/2};
			if (center < envI->rect)
			{
				if (envI->sides[0])
					this->externVel.y -= envI->boostMag * LAUNCH_BASE_VALUE;
				else if (envI->sides[2])
					this->externVel.y += envI->boostMag * LAUNCH_BASE_VALUE;

				if (envI->sides[1])
					this->externVel.x += envI->boostMag * LAUNCH_BASE_VALUE;
				else if (envI->sides[3])
					this->externVel.x -= envI->boostMag * LAUNCH_BASE_VALUE;
			}

			// TODO add a player flag to indicate the player is being launched (unique animation perhaps)

			continue;
		}
		else if (envI->type == HAZARD && !godmode)
		{
			// collide with a spike? DIE
			if (OverlapBox(playerRect, envI->rect, &overlap))
			{
				this->isDead = true;
				return;
			}
		}
		else if (envI->type == GOAL)
		{
			Vector2 center = Vector2{this->pos.x, this->pos.y - hitboxSize.y/2};
			if (center < envI->rect)
				inEndZone = true;
			else
				inEndZone = false;
		}
		else if (envI->type == BLOCK || (envI->type == HAZARD && godmode))
		{
			// TODO consider adding a check to exclude objects that are super far away

			if (OverlapBox(playerRect, envI->rect, &overlap))
			{
				Vector2 offset = {0,0};
				
				// bool up_stairs = false;
				if (overlap.height <= PLAYER_STAIR_HEIGHT && (overlap.y > playerRect.y) && abs(vel.x) > 0)
				{
					offset.y -= overlap.height;
					vel.y = 0;
					temp_flag_ON_GROUND = true;
					// up_stairs = true;
				}
				else if (overlap.height < PLAYER_TERMINAL_VELOCITY)
				{
					if ((overlap.y > playerRect.y && vel.y > 0) && envI->sides[0])// || (abs(vel.x > 0) && overlap.height <= STAIR_HEIGHT))
					{
						offset.y -= overlap.height;
						vel.y = 0;
						temp_flag_ON_GROUND = true;
					}
					else if ((overlap.y == playerRect.y && vel.y < 0) && envI->sides[2])
					{
						offset.y += overlap.height;
						vel.y = 0;
					}
				}
				// if ((overlap.width <= overlap.height) && !up_stairs)// || (vel.y > 10)) // DEPRECATEDCODE_02
				if ((overlap.width <= overlap.height))// || (vel.y > 10)) // DEPRECATEDCODE_02
				{ // DEPRECATEDCODE_02
					// std::cout << "Second case hit\n";

					if (overlap.width < 0.5f * playerRect.width)
					{
						if((overlap.x > playerRect.x && vel.x > 0) && envI->sides[3])
						{
							offset.x -= overlap.width;
							vel.x = 0;
							temp_flag_ON_WALL_RIGHT = true;
						}
						else if ((overlap.x == playerRect.x && vel.x < 0) && envI->sides[1])
						{
							offset.x += overlap.width;
							vel.x = 0;
							temp_flag_ON_WALL_LEFT = true;
						}
					}
				} // DEPRECATEDCODE_02

				// if (up_stairs)
				// {
				// 	// small penalty to velocity
				// 	std::cout << "Up Stairs\n";
				// 	vel.x *= PLAYER_STAIR_PENALTY;
				// }
				// else
				// 	std::cout << "\n";

				pos = pos + offset;
			}

			continue;
		} /// end BLOCK case


	}// end for loop

	/// UPDATE PLAYER STATE FLAGS
	if (temp_flag_ON_GROUND && this->vel.y == 0)
	{
		_flag[IS_JUMPING] = false;
		_flag[ON_GROUND] = true;
	}
	else
		_flag[ON_GROUND] = false;

	/// RIGHT WALL
	if (temp_flag_ON_WALL_RIGHT)
	{
		_flag[IS_JUMPING] = false;
		_flag[ON_WALL_RIGHT] = true;
	}
	else
	{
		_flag[ON_WALL_RIGHT] = false;
		_value[ON_WALL_RIGHT] = 0;
	}

	/// LEFT WALL
	if (temp_flag_ON_WALL_LEFT)
	{
		_flag[IS_JUMPING] = false;
		_flag[ON_WALL_LEFT] = true;
	}
	else
	{
		_flag[ON_WALL_LEFT] = false;
		_value[ON_WALL_LEFT] = 0;
	}

}

void player::update(env_list* env)
{
	move();
	check(env);
}

void player::DrawPlayer()
{
	float w = hitboxSize.x;
	float h = hitboxSize.y;

	// dev stand-in for a real render of sorts
	Rectangle r = Rectangle{pos.x - w/2, pos.y - h, w, h};
	DrawRectangleRec(r, color);
	DrawText(TextFormat("%d", coins), (int)pos.x - 44, (int)pos.y - 95, 30, NEARBLACK);
}

void player::setHitboxSize(Vector2 hitboxSize)
{
	this->hitboxSize = hitboxSize;

	this->hitbox_corners[0] = Vector2{0 - this->hitboxSize.x/2, 0 - hitboxSize.y - 1};
	this->hitbox_corners[1] = Vector2{hitboxSize.x/2 + 1, 0 - hitboxSize.y};
	this->hitbox_corners[2] = Vector2{hitboxSize.x/2, 1};
	this->hitbox_corners[3] = Vector2{0 - hitboxSize.x/2 - 1, 0};
}

bool coinDist(player pl, Rectangle coinRect)
{
	Vector2 diff = Vector2{pl.pos.x, (pl.pos.y - pl.hitboxSize.y/2)} - (Vector2{(coinRect.x + coinRect.width/2), (coinRect.y + coinRect.height/2)});
	diff.x = (diff.x < 0) ? -diff.x : diff.x;
	diff.y = (diff.y < 0) ? -diff.y : diff.y;
	return diff.x < PLAYER_COIN_RADIUS && diff.y < PLAYER_COIN_RADIUS;

	// Vector2 plV = pl.pos;
	// plV = plV - Vector2{0, pl.hitboxSize.y/2};
	// float a = plV.x - (coinRect.x + coinRect.width/2);
	// float b = plV.y - (coinRect.y + coinRect.height/2);
	// if (a < 0) a *= (-1);
	// if (b < 0) b *= (-1);

	// return a < PLAYER_COIN_RADIUS && b < PLAYER_COIN_RADIUS;
}

static bool OverlapBox(Rectangle a, Rectangle b, Rectangle* out)
{
	/* b > a 		logically equivalent to 
	b - a > 0
	plus we will use b - a so we store these and check against 0 instead
	*/
	// return (((b.x < a.x + a.width) && (b.x + b.width > a.x)) && ((b.y < a.y + a.height) && (b.y + b.height > a.y)));
	float awb = a.x + a.width - b.x; // b.x < a.x + a.width
	float bwa = b.x + b.width - a.x; // b.x + b.width > a.x
	float ahb = a.y + a.height - b.y; // b.y < a.y + a.height
	float bha = b.y + b.height - a.y; // b.y + b.height - a.y

	if (awb > 0 && bwa > 0 && ahb > 0 && bha > 0)
	{
		*out = Rectangle{
			(a.x < b.x) ? b.x : a.x,
			(a.y < b.y) ? b.y : a.y,
			(a.x < b.x) ? MIN(b.width, awb) : MIN(a.width, bwa),
			(a.y < b.y) ? MIN(b.height, ahb) : MIN(a.height, bha)
		};
		return true;
	}
	else
		return false;
}

void player::reset()
{
	this->isDead = false;
	this->inEndZone = false;
	this->coins = 0;
	this->pos = current_level.player_start;
	this->acc = this->vel = Vector2{0,0};
	this->externAcc = this->externVel = Vector2{0,0};

	for(int i = 0; i < PLAYER_FLAG_COUNT; i++)
		this->_flag[i] = false;

	for(int i = 0; i < PLAYER_FLAG_COUNT; i++)
		this->_value[i] = 0;
}