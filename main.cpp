#include "raylib.h"
#define NEARBLACK CLITERAL(Color){ 20, 20, 20, 255}
#define MUSTARD CLITERAL(Color){ 203, 182, 51, 255}

#include "constants.h"

#include "environment.h"
#include "player.h"

void InitializeLevel(int level, env_list* env);
void InitializeLevels(std::vector<env_level>* levels);

void UpdateLevel(env_level* level);
void DrawLevel(env_level level);

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

	InitializeLevels(&levels);
	//}

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
        camera.zoom += ((float)GetMouseWheelMove()*0.05f);

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.25f) camera.zoom = 0.25f;

        if (IsKeyPressed(KEY_R))
        {
            camera.zoom = 1.0f;
        }
		//}

		//{ LEVEL UPDATE
		UpdateLevel(&levels[level]);
		//}

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            //DrawTextRec(GetFontDefault(), "Congrats! You created your first window!", Rectangle{200, 250, 400, 100}, 20, 1.0, true, LIGHTGRAY);

            BeginMode2D(camera);

                //{ LEVEL DRAW
                DrawLevel(levels[level]);
                //}

				/// draw player
				p1.DrawPlayer();

			EndMode2D();

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

void InitializeLevel(int level, env_list* env)
{
    env->clear();
    env_object obj;

    switch(level)
    {
    case 0:
        obj = env_object{Rectangle{0,600, 800, 100}};
        obj.type = BLOCK;
        env->push_back(obj);

        obj = env_object{Rectangle{350,500,200,100}};
        obj.type = BLOCK;
        env->push_back(obj);
        break;
    default:
        obj = env_object{Rectangle{0,600, 800, 100}};
        obj.type = BLOCK;
        env->push_back(obj);

        obj = env_object{Rectangle{300,200,200,200}};
        obj.type = TEXT;
        obj.label = "I'm sorry, but your princess\nis in another castle.";
        env->push_back(obj);
        break;
    }
}

void InitializeLevels(std::vector<env_level>* levels)
{
    levels->clear();

    env_list env;
    env_level lev;

    for(int i = 0; i < 2; i++)
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
