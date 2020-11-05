/// TODO
/// - implement LineCheck function for collision checking
/// -
/// - use nlohmann/json for the following
/// 	- procedure for writing a level to a json
///		- procedure for reading a level from a json
///		- consider storing many levels in a json?
/// - create system to read levels from file
/// - create level editor


#include "raylib.h"
#define NEARBLACK CLITERAL(Color){ 20, 20, 20, 255}
#define MUSTARD CLITERAL(Color){ 203, 182, 51, 255}

#define DEV_TEST_LEVEL

#include "constants.h"

#include "environment.h"
#include "player.h"

#include "files.h"

// DEV SHIT
#include <iostream>

void InitializeLevel(int level, env_list* env);
void InitializeLevels(std::vector<env_level>* levels);

void UpdateLevel(env_level* level);
void DrawLevel(env_level level);
void ResetLevel(player*, env_level);

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

	/////////////////////////////////////////////////////////////
    // TODO: Load resources / Initialize variables at this point
    /////////////////////////////////////////////////////////////

	//{ INIT PLAYER
	player p1(Vector2{screenWidth/5, screenWidth/2});
	//}

	//{ init env_list (truly init per level)
	std::vector<env_level> levels;
	unsigned short level = 0; /**< acts as an index to std::vector<> levels. level 0 is an interactive menu, level 1 marks the first level in the game. */

	// if we're in DEV_TEST mode then it will simply load test_level.json in the levels directory and play that. in this case pressing r will also "restart" the level
	#ifdef DEV_TEST_LEVEL

	levels.push_back(LoadLevelFromFile("test_level.json"));

	#else

	InitializeLevels(&levels);

	#endif // DEV_TEST_LEVEL

	ResetLevel(&p1, levels[level]);

	/// DEV SHIT

	/// set fps of window
    SetTargetFPS(TARGET_FPS);

	/// remove exit key functionality (alt-f4 will always work)
	SetExitKey(-1);

	/// Create the camera object
	Camera2D camera{Vector2{0,0}, Vector2{0,0}, 0, 1.0f};
	//camera.offset = Vector2{screenWidth/2, screenHeight/2 - 50};

    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update variables / Implement example logic at this point
        //----------------------------------------------------------------------------------

		p1.update(levels[level].env_objects);

		//{ CAMERA UPDATE
        camera.zoom += ((float)GetMouseWheelMove()*0.005f);
        camera.target = p1.pos + Vector2{0, p1.hitboxSize.y / 2};
        camera.offset = Vector2{400, 300 + p1.hitboxSize.y / 2};
        //camera.rotation += 0.5f;

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.25f) camera.zoom = 0.25f;

        if (IsKeyPressed(KEY_R))
        {
            camera.zoom = 1.0f;
            #ifdef DEV_TEST_LEVEL

			levels.clear();
			levels.push_back(LoadLevelFromFile("levels/test_level.json"));
			ResetLevel(&p1, levels[0]);

            #endif // DEV_TEST_LEVEL
        }
		//}

		//{ LEVEL UPDATE
		UpdateLevel(&levels[level]);
		//}

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode2D(camera);

                //{ LEVEL DRAW
                DrawLevel(levels[level]);
                //}

				/// draw player
				p1.DrawPlayer();

			EndMode2D();

			Vector2 v = GetScreenToWorld2D(GetMousePosition(), camera);
			DrawText(FormatText("%.2f %.2f", v.x, v.y), GetMouseX(), GetMouseY() + 25, 20, NEARBLACK);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    // TODO: Unload all loaded resources at this point

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void InitializeLevel(unsigned short level, env_list* env)
{
    env->clear();
    env_object obj;

    switch(level)
    {
    case 0:
        obj = env_object{Rectangle{0,600, 800, 100}, 0};
        obj.type = BLOCK;
        env->push_back(obj);

        obj = env_object{Rectangle{300,200,200,200}, 1};
        obj.type = TEXT;
        obj.label = "I'm sorry, but your princess\nis in another castle.";
        env->push_back(obj);
        break;
    case 1:
        obj = env_object{Rectangle{-1000,600, 2800, 100}, 0};
        env->push_back(obj);

        obj = env_object{Rectangle{350,300,200,300}, 1};
        env->push_back(obj);

        obj = env_object{Rectangle{500,500,200,100}, 2};
        env->push_back(obj);

        obj = env_object{Rectangle{200,400,200,100}, 3};
        env->push_back(obj);

        obj = env_object{Rectangle{-400,300, 200,300}, 4};
        obj.setSides(true, true, false, false);
        obj.color = GREEN;
        env->push_back(obj);

        obj = env_object{Rectangle{0,100, 200,300}, 5};
        obj.setSides(true, true, true, true);
        obj.color = GREEN;
        env->push_back(obj);

        obj = env_object{Rectangle{450,200, 100,100}, 6};
        env->push_back(obj);

        obj = env_object{Rectangle{-400,0, 200,100}, 7};
        obj.setSides(true, true, true, true);
        env->push_back(obj);

        break;
    default:
        obj = env_object{Rectangle{0,600, 800, 100}, 0};
        obj.type = BLOCK;
        env->push_back(obj);

        obj = env_object{Rectangle{300,200,200,200}, 1};
        obj.type = TEXT;
        obj.label = "I'm sorry, but your princess is in another castle.";
        env->push_back(obj);
        break;
    }
}

void InitializeLevels(std::vector<env_level>* levels)
{
    levels->clear();

    env_list env;
    env_level lev;

    for(unsigned short i = 0; i < 3; i++)
    {
        InitializeLevel(i, &env);
        lev = env_level{i, "Level " + std::to_string(i), env};
        levels->push_back(lev);
    }
}

void UpdateLevel(env_level* level)
{
    auto i = level->env_objects.begin(), iE = level->env_objects.end();
    while(i != iE)
    {
        if (i->func != STATIC)
        {
            // do some update on that object
        }
        i++;
    }
}

void DrawLevel(env_level level)
{
    DrawEnvList(level.env_objects);
}

void ResetLevel(player* p, env_level level)
{
	p->acc = p->vel = Vector2{0,0};
	p->pos = level.player_start;
}
