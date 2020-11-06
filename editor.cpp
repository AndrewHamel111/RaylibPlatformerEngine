/// TODO
/// - create level editor (this will be the main file for the editor)

#include "raylib.h"
#define NEARBLACK CLITERAL(Color){ 20, 20, 20, 255}
#define MUSTARD CLITERAL(Color){ 203, 182, 51, 255}

#define DEV_SHOW_MOUSE_POS

#define CAMERA_SCROLL_SPEED 6.0f

#include "environment.h"
#include "operators.h"

#include "files.h"

// DEV SHIT
#include <iostream>

void UpdateLevel(env_level* level);
void DrawLevel(env_level level);

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1200;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Level Editor");

	/////////////////////////////////////////////////////////////
    // TODO: Load resources / Initialize variables at this point
    /////////////////////////////////////////////////////////////

    bool levelIsLoaded = false;
    std::string file_name;
    std::string window_label = "Level Tester";
    env_level level;


	/// set fps of window
    SetTargetFPS(90);

	/// remove exit key functionality (alt-f4 will always work)
	SetExitKey(-1);

	/// Create the camera object
	Camera2D camera{Vector2{0,0}, Vector2{0, 0}, 0, 0.5f};
	camera.offset = {550, 300};
	//camera.offset = Vector2{screenWidth/2, screenHeight/2 - 50};

    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update variables / Implement example logic at this point
        //----------------------------------------------------------------------------------

        camera.zoom += ((float)GetMouseWheelMove()*0.005f);
        //camera.rotation += 0.5f;

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.25f) camera.zoom = 0.25f;

        if (IsKeyPressed(KEY_R))
        {
            camera.zoom = 1.0f;
        }

        float scrollSpeed = CAMERA_SCROLL_SPEED;
        if (IsKeyDown(KEY_LEFT_SHIFT)) scrollSpeed *= 3;

		// WASD scrolls camera
		camera.target = camera.target + (scrollSpeed * Vector2{(IsKeyDown(KEY_D) - IsKeyDown(KEY_A)), (IsKeyDown(KEY_S) - IsKeyDown(KEY_W))});

		// mouse wheel zooms in and out

        /// TODO guard this action with a prompt (so the user can't just instantly lose
		/// the level they were working on
        if (IsFileDropped())
		{
			int count;
			char** files = GetDroppedFiles(&count);
			if (count > 0)
				file_name = files[0];

			level = LoadLevelFromFile(file_name);
			ClearDroppedFiles();
			SetWindowTitle(TextFormat("%s %s", "Editing ", GetFileName(file_name.c_str())));
			levelIsLoaded = true;
		}

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(WHITE);

            BeginMode2D(camera);

                //{ LEVEL DRAW
                if (levelIsLoaded)
					DrawLevel(level);
                //}

			EndMode2D();

            // draw edit panel
            DrawRectangle(800, 0, 400, 600, RAYWHITE);

			// draw buttons

			// draw grid -+ buttons (alternative square brackets)

			// draw block size -+ buttons (alternative arrow keys)

			// jump to coordinates button

			// name field, save and load buttons


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
