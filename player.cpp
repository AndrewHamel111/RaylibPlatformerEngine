#include "player.h"

/// CONSTRUCTORS

player::player(): pos{Vector2{400,300}}, vel{Vector2{0,0}}, acc{Vector2{0,0}}, hitboxSize{Vector2{100,100}}
{
	// DEFAULT CONSTRUCTOR

    // ADD CORNERS AND EDGES TO ANCHORS
    this->hitbox_anchors.clear();
    this->hitbox_anchors.push_back(Vector2{0, -101});
    this->hitbox_anchors.push_back(Vector2{51, -101});
    this->hitbox_anchors.push_back(Vector2{51, -50});
    this->hitbox_anchors.push_back(Vector2{51, 1});
    this->hitbox_anchors.push_back(Vector2{0, 1});
    this->hitbox_anchors.push_back(Vector2{-51, 1});
    this->hitbox_anchors.push_back(Vector2{-51, -50});
    this->hitbox_anchors.push_back(Vector2{-51, -101});
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
	if (IsKeyPressed(KEY_SPACE) && !_flag[IS_JUMPING])
	{
		// normal jump
		if (_flag[ON_GROUND])
		{
			vel.y = (-1) * PLAYER_JUMP_VELOCITY;
			_flag[IS_JUMPING] = true;
		}
		// wall jump (from left wall)
		else if (_flag[ON_WALL_LEFT])
		{
			vel.y = (-1) * PLAYER_JUMP_VELOCITY;
			vel.x =  PLAYER_WALL_JUMP_MODIFIER * PLAYER_JUMP_VELOCITY;
		}
		// wall jump (from right wall)
		else if (_flag[ON_WALL_RIGHT])
		{
			vel.y = (-1) * PLAYER_JUMP_VELOCITY;
			vel.x = (-1) * PLAYER_WALL_JUMP_MODIFIER * PLAYER_JUMP_VELOCITY;
			_flag[IS_JUMPING] = true;
		}
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

void player::check(env_list env)
{
	// iterators and a counter
    auto envI = env.begin(), envIE = env.end();
    auto i = hitbox_anchors.begin(), iE = hitbox_anchors.end();

    unsigned short _i = 0;

    bool temp_flag_ON_GROUND = false;
    bool temp_flag_ON_WALL_LEFT = false;
    bool temp_flag_ON_WALL_RIGHT = false;

    // for each env_object
    while (envI != envIE)
    {
        if ((this->pos + Vector2{0, -50}) < envI->rect)
		{
			envI++;
			continue;
		}

        i = hitbox_anchors.begin();
        _i = 0;
		bool anchor_flags[8] = {false};

        // for each player hitbox anchor
        while(i != iE)
        {
        	// trip the corresponding flags
			if (this->pos + *i < envI->rect)
			{
				anchor_flags[_i] = true;
				std::cout << "stale cum " << std::to_string(_i) << std::endl;
			}

			_i++;
			i++;
        }

        if (envI->sides[2] && ((anchor_flags[0] && anchor_flags[1]) || (anchor_flags[7] && anchor_flags[0])))
		{
			// IN CEILING

			// if still moving upward set vel to 0
			if (this->vel.y < 0)
			{
				this->pos.y = envI->rect.y + envI->rect.height + this->hitboxSize.y;
				this->vel.y = 0;
			}
		}
		if (envI->sides[3] && ((anchor_flags[1] && anchor_flags[2]) || (anchor_flags[2] && anchor_flags[3])))
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
		if (envI->sides[0] && ((anchor_flags[3] && anchor_flags[4]) || (anchor_flags[4] && anchor_flags[5])))
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
		if (envI->sides[1] && ((anchor_flags[5] && anchor_flags[6]) || (anchor_flags[6] && anchor_flags[7])))
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

    // TODO add a grace period for wall jumps (player can hang on wall for a moment, perhaps slowly sliding down

    if (temp_flag_ON_GROUND && this->vel.y == 0)
	{
		_flag[IS_JUMPING] = false;
		_flag[ON_GROUND] = true;
	}
	else
		_flag[ON_GROUND] = false;

    if (temp_flag_ON_WALL_RIGHT)
	{
		_flag[IS_JUMPING] = false;
		_flag[ON_WALL_RIGHT] = true;
	}
	else
		_flag[ON_WALL_RIGHT] = false;

    if (temp_flag_ON_WALL_LEFT)
	{
		_flag[IS_JUMPING] = false;
		_flag[ON_WALL_LEFT] = true;
	}
	else
		_flag[ON_WALL_LEFT] = false;

}

void player::correct()
{
	// TODO : this will take a lot of work lmfao
	// might just be a part of the check function
}

void player::update(env_list env)
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
	DrawRectangleRec(r, Color{20,20,20,60});

	//std::string s = "vel " + std::to_string(vel.x) + " " + std::to_string(vel.y) +"\nacc " + std::to_string(acc.x) + " " + std::to_string(acc.y);
	//DrawText(s.c_str(), 0, 0, 20, NEARBLACK);

	//if (_flag[ON_GROUND]) DrawText("GROUNDED", 0, 200, 20, NEARBLACK);

	//{ DRAW HITBOX POINTS

	//auto i = hitbox_anchors.begin(), iE = hitbox_anchors.end();
	//while(i != iE)
        //DrawCircleV(pos + *(i++),2, BLUE);

	//}
}
