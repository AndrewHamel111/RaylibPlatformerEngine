#include "player.h"

/// CONSTRUCTORS

player::player(): pos{Vector2{400,300}}, vel{Vector2{0,0}}, acc{Vector2{0,0}}
{
	// DEFAULT CONSTRUCTOR
}

player::player(Vector2 pos): pos{pos}, vel{Vector2{0,0}}, acc{Vector2{0,0}}
{
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
	acc.y = PLAYER_GRAVITY;
	
	// set acc based on input
	if (IsKeyDown(KEY_D))
		acc.x = PLAYER_BASE_ACCELERATION;
	else if (IsKeyDown(KEY_A))
		acc.x = (-1) * PLAYER_BASE_ACCELERATION;
	else
		acc.x = 0;
	
	// apply acc (when applicable)
	if (abs(vel.x) < PLAYER_MAX_VELOCITY)// || abs(vel.x + acc.x) < PLAYER_MAX_VELOCITY)
		vel.x += acc.x;
	
	vel.y += acc.y;
	
	// add jump (when applicable)
	if (IsKeyDown(KEY_SPACE) && !_flag[IS_JUMPING])
	{
		vel.y = (-1) * PLAYER_JUMP_VELOCITY;
		_flag[IS_JUMPING] = true;
	}
	
	//////////////////
	//  UPDATE POSITION
	pos = pos + vel;
	//////////////////
	
	// apply dec
	if (vel.x != 0)
	{
		float decel = PLAYER_DECELERATION;
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
}

void player::check(env_list* env)
{
	// VERY TEMPORARY
	if (pos.y > 600)
	{
		// bump out of ground and stop the falling speed.
		pos.y = 600;
		vel.y = 0;
		
		// allow a jump since we've touched ground
		_flag[IS_JUMPING] = false;
		_flag[ON_GROUND] = true;
	}
	else
		_flag[ON_GROUND] = false;
}

void player::correct()
{
	// TODO : this will take a lot of work lmfao
	// might just be a part of the check function
}

void player::update(env_list* env)
{
	move();
	check(env);
	//correct();
}

void player::DrawPlayer()
{	
	float w = 100;
	float h = 100;
	
	// dev stand-in for a real render of sorts
	Rectangle r = Rectangle{pos.x - w/2, pos.y - h, w, h};
	DrawRectangleRec(r, NEARBLACK);
	
	std::string s = "vel " + std::to_string(vel.x) + " " + std::to_string(vel.y) +"\nacc " + std::to_string(acc.x) + " " + std::to_string(acc.y);
	DrawText(s.c_str(), 0, 0, 20, NEARBLACK);
	
	if (_flag[ON_GROUND]) DrawText("GROUNDED", 0, 200, 20, NEARBLACK);
}