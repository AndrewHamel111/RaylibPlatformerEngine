#include "player.h"

/// CONSTRUCTORS

player::player(): pos{Vector2{400,300}}, vel{Vector2{0,0}}, acc{Vector2{0,0}}, hitboxSize{Vector2{100,100}}, coins{0}, color{RandomColor()}
{
	// DEFAULT CONSTRUCTOR

	// ADD CORNERS AND EDGES TO ANCHORS
	/// TODO change hitbox anchors to be corners of hitbox (propagate change to LineCheck)
	this->hitbox_anchors.clear();
	this->hitbox_anchors.push_back(Vector2{0, -101});
	this->hitbox_anchors.push_back(Vector2{51, -101});
	this->hitbox_anchors.push_back(Vector2{51, -50});
	this->hitbox_anchors.push_back(Vector2{51, 1});
	this->hitbox_anchors.push_back(Vector2{0, 1});
	this->hitbox_anchors.push_back(Vector2{-51, 1});
	this->hitbox_anchors.push_back(Vector2{-51, -50});
	this->hitbox_anchors.push_back(Vector2{-51, -101});

	// top left, top right, bottom right, bottom left
	this->hitbox_corners[0] = Vector2{-50, -101};
	this->hitbox_corners[1] = Vector2{51, -100};
	this->hitbox_corners[2] = Vector2{50, 1};
	this->hitbox_corners[3] = Vector2{-51, 0};
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

	// set acc based on input
	/*
	if (IsKeyDown(KEY_D))
		acc.x = accel;
	else if (IsKeyDown(KEY_A))
		acc.x = (-1) * accel;
	else
		acc.x = 0;
		*/
	acc.x = accel * (IsKeyDown(KEY_D) - IsKeyDown(KEY_A));

	// apply acc (when applicable)
	if (abs(vel.x + acc.x ) < maxVel)// || abs(vel.x + acc.x) < PLAYER_MAX_VELOCITY)
		vel.x += acc.x;
	//else
	//vel.x = maxVel * (vel.x > 0) ? 1 : -1;

	if (abs(vel.y + acc.y) < PLAYER_TERMINAL_VELOCITY)
		vel.y += acc.y;
	//vel.y = PLAYER_TERMINAL_VELOCITY * (vel.y > 0) ? 1 : -1;

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
}

void player::check(env_list* env)
{
	// iterators and a counter
	auto envI = env->begin();

	bool temp_flag_ON_GROUND = false;
	bool temp_flag_ON_WALL_LEFT = false;
	bool temp_flag_ON_WALL_RIGHT = false;

	const int lineSegmentCount = 10;

	// for each env_object
	while (envI != env->end())
	{
		if (envI->type == TEXT)
		{
			envI++;
			continue;
		}
		if (envI->type == COIN)
		{
			if (coinDist(this->pos, envI->rect))
			{
				envI->isCollected = true;
				this->coins++;
			}

			envI++;
			continue;
		}

		// don't bother with collision check if our center is totally inside of the current object,
		// since it's most likely a semisolid that the player is "in front of"
		if ((this->pos + Vector2{0, -50}) < envI->rect)
		{
			envI++;
			continue;
		}

		if (envI->sides[2] && LineCheck(UP, lineSegmentCount, *envI))
		{
			// IN CEILING

			// if still moving upward set vel to 0
			if (this->vel.y < 0)
			{
				this->pos.y = envI->rect.y + envI->rect.height + this->hitboxSize.y;
				this->vel.y = 0;
			}
		}
		if (envI->sides[3] && LineCheck(RIGHT, lineSegmentCount, *envI))
		{
			// WALL ON RIGHT

			// if still moving right set vel to 0
			if (this->vel.x > 0)
			{
				this->pos.x = envI->rect.x - this->hitboxSize.x/2;
				this->vel.x = 0;
			}

			temp_flag_ON_WALL_RIGHT = true;
		}
		if (envI->sides[0] && LineCheck(DOWN, lineSegmentCount, *envI))
		{
			// IN FLOOR

			// if still falling set vel to 0
			if (this->vel.y > 0)
			{
				this->pos.y = envI->rect.y;
				this->vel.y = 0;
			}

			temp_flag_ON_GROUND = true;
		}
		if (envI->sides[1] && LineCheck(LEFT, lineSegmentCount, *envI))
		{
			// WALL ON LEFT

			// if still moving right set vel to 0
			if (this->vel.x < 0)
			{
				this->pos.x = envI->rect.x + envI->rect.width + this->hitboxSize.x/2;
				this->vel.x = 0;
			}

			temp_flag_ON_WALL_LEFT = true;
		}

		envI++;
	}

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
	float w = 100;
	float h = 100;

	// dev stand-in for a real render of sorts
	Rectangle r = Rectangle{pos.x - w/2, pos.y - h, w, h};
	DrawRectangleRec(r, color);
	DrawText(FormatText("%d", coins), (int)pos.x - 44, (int)pos.y - 95, 30, NEARBLACK);
}


/*	Vector2{-51, -101}		Vector2{0, -101} 	Vector2{51, -101}
	Vector2{-51, -50}							Vector2{51, -50}
	Vector2{-51, 1}			Vector2{0, 1}		Vector2{51, 1} */
/// TODO improve this shit because it's certainly not divine intellect
bool player::LineCheck(LineCheckDirection dir, int lineSegments, env_object obj)
{
	// never use less than 5 line segments (would be very inaccurate if we did)
	if (lineSegments < 5) lineSegments = 5;

	const float hitPercentage = PLAYER_HITBOX_PERCENTAGE;

	unsigned char collisionCounter = 0;

	switch(dir)
	{
	case UP: // top left to top right
		for(int i = 0; i < lineSegments && (float)collisionCounter/lineSegments < hitPercentage; i++)
		{
			// player's position + the offset to reach the appropriate corner (top left) + the offset from where we are in the iteration
			Vector2 hitPoint = this->pos + this->hitbox_corners[0] + Vector2{(float)i/(lineSegments - 1)*(this->hitboxSize.x - 1), 0};
			if (hitPoint < obj.rect && this->pos.y > (obj.rect.y + obj.rect.height)) ///TODO double check this if
				collisionCounter++;
		}
		break;
	case RIGHT: // top right to bottom right
		for(int i = 0; i < lineSegments && (float)collisionCounter/lineSegments < hitPercentage; i++)
		{
			Vector2 hitPoint = this->pos + this->hitbox_corners[1] + Vector2{0, (float)i/(lineSegments - 1)*(this->hitboxSize.y - 1)};
			if (hitPoint < obj.rect && this->pos.x < obj.rect.x)
				collisionCounter++;
		}
		break;
	case DOWN: // bottom right to bottom left
		for(int i = 0; i < lineSegments && (float)collisionCounter/lineSegments < hitPercentage; i++)
		{
			Vector2 hitPoint = this->pos + this->hitbox_corners[2] - Vector2{(float)i/(lineSegments - 1)*(this->hitboxSize.x - 1), 0};
			if (hitPoint < obj.rect && this->pos.y < (obj.rect.y + this->hitboxSize.y/2))
				collisionCounter++;
		}
		break;
	case LEFT: // bottom left to top right
		for(int i = 0; i < lineSegments && (float)collisionCounter/lineSegments < hitPercentage; i++)
		{
			Vector2 hitPoint = this->pos + this->hitbox_corners[3] - Vector2{0, (float)i/(lineSegments - 1)*(this->hitboxSize.y - 1)};
			if (hitPoint < obj.rect && this->pos.x > (obj.rect.x + obj.rect.width))
				collisionCounter++;
		}
		break;
	}

	if ((float)collisionCounter/lineSegments >= hitPercentage)
		return true;
	return false;
}

bool coinDist(Vector2 plV, Rectangle coinRect)
{
	plV = plV - Vector2{0, 50};
	float a = plV.x - (coinRect.x + coinRect.width/2);
	float b = plV.y - (coinRect.y + coinRect.height/2);
	if (a < 0) a *= (-1);
	if (b < 0) b *= (-1);

	return a < PLAYER_COIN_RADIUS && b < PLAYER_COIN_RADIUS;
}
