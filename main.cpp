/// TODO
/// - create level editor
/// - when in the level testing mode, instead of trying to load
///     some preset value it waits for the user to drag and drop a level on the editor.


#include "raylib.h"
#define NEARBLACK CLITERAL(Color){ 20, 20, 20, 255}
#define MUSTARD CLITERAL(Color){ 203, 182, 51, 255}

#define DEV_SHOW_MOUSE_POS

#include "constants.h"

#include "environment.h"
#include "player.h"

#include "files.h"

// DEV SHIT
#include <iostream>

void UpdateLevel(env_level* level);
void DrawLevel(env_level level);
void ResetLevel(player*, env_level);

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 600;

	player p1(Vector2{0,0});

	#ifdef DEV_LEVEL_TEST

    bool levelIsLoaded = false;
    std::string file_name;
    std::string window_label = "Level Tester";
    env_level level;

	#else

    std::string window_label = "Platformer Engine";
	std::vector<env_level> levels;
	unsigned short level = 0; /**< acts as an index to std::vector<> levels. level 0 is an interactive menu, level 1 marks the first level in the game. */
    // load all levels in the levels folder
    try
    {
        levels = LoadLevelsFromFolder("levels");
    }
    catch (JSONLevelException except)
    {
        /// TODO handle the case where there is an invalid level in levels/
        exit(0);
    }
    catch (std::string msg)
    {
        /// TODO handle the case where the folder has no files in it whatsoever
        exit(0);
    }
	ResetLevel(&p1, levels[level]);

	#endif // DEV_LEVEL_TEST

    InitWindow(screenWidth, screenHeight, window_label.c_str());

	/////////////////////////////////////////////////////////////
    // TODO: Load resources / Initialize variables at this point
    /////////////////////////////////////////////////////////////

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

        #ifdef DEV_LEVEL_TEST
        if (levelIsLoaded)
            p1.update(level.env_objects);
        #else
		p1.update(levels[level].env_objects);
        #endif

		/// CAMERA UPDATE
        camera.zoom += ((float)GetMouseWheelMove()*0.005f);
        camera.target = p1.pos + Vector2{0, p1.hitboxSize.y / 2};
        camera.offset = Vector2{400, 300 + p1.hitboxSize.y / 2};

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.25f) camera.zoom = 0.25f;

        if (IsKeyPressed(KEY_R))
        {
            camera.zoom = 1.0f;

            #ifdef DEV_LEVEL_TEST
			level = LoadLevelFromFile(file_name);
			ResetLevel(&p1, level);
            #endif // DEV_TEST_LEVEL
        }

		/// LEVEL UPDATE
        #ifdef DEV_LEVEL_TEST
        if (IsKeyPressed(KEY_ESCAPE))
        {
            levelIsLoaded = false;
            file_name = "";
            SetWindowTitle("Level Tester");
        }
        UpdateLevel(&level);
        if (IsFileDropped())
		{
			int count;
			char** files = GetDroppedFiles(&count);
			if (count > 0)
				file_name = files[0];

			level = LoadLevelFromFile(file_name);
			ClearDroppedFiles();
			SetWindowTitle(GetFileName(file_name.c_str()));
			ResetLevel(&p1, level);
			levelIsLoaded = true;
		}
        #else
		UpdateLevel(&levels[level]);
        #endif

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            #ifdef DEV_LEVEL_TEST
            if (!levelIsLoaded)
            {
                DrawTextRec(GetFontDefault(), "DRAG AND DROP A LEVEL ON THIS WINDOW TO LOAD IT", Rectangle{200, 150, 400, 400}, 30, 1.0, true, LIGHTGRAY);

                /*
                if (IsFileDropped())
                {
                    int count;
                    char** files = GetDroppedFiles(&count);
                    if (count > 0)
                        file_name = files[0];

                    level = LoadLevelFromFile(file_name);
                    ClearDroppedFiles();
                    SetWindowTitle(GetFileName(file_name.c_str()));
			        ResetLevel(&p1, level);
                    levelIsLoaded = true;
                }
                */

                EndDrawing();
                continue;
            }
            #endif
            BeginMode2D(camera);

                #ifdef DEV_LEVEL_TEST
                DrawLevel(level);
                #else
                DrawLevel(levels[level]);
                #endif

				/// draw player
				p1.DrawPlayer();

			EndMode2D();

            #ifdef DEV_SHOW_MOUSE_POS
			Vector2 v = GetScreenToWorld2D(GetMousePosition(), camera);
			DrawText(FormatText("x=%4.2f\ny=%4.2f", v.x, v.y), GetMouseX(), GetMouseY() + 20, 20, NEARBLACK);
            #endif

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
