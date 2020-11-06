/// TODO
/// - create level editor (this will be the main file for the editor)

#include "raylib.h"
#define NEARBLACK CLITERAL(Color){ 20, 20, 20, 255}
#define MUSTARD CLITERAL(Color){ 203, 182, 51, 255}

#define DEV_SHOW_MOUSE_POS

#define CAMERA_SCROLL_SPEED 6.0f
#define EDITOR_RESIZE_SPEED 25.0f

#include "environment.h"
#include "operators.h"
#include "button.h"

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

    /// LOAD BUTTONS AND ASSIGN SOURCE/DEST RECTS
	Texture2D buttonTex = LoadTexture("sprites/buttons.png");
	bool buttonValues[8] = {false};
	short editorFlag = -1;
	enum BUTTON_SOURCE_RECT {
		BLOCK, SIDES, DELETE, PAINT, TEMP, TEMP1, TEMP2, SAVE
	};
	Rectangle buttonSourceRect[] = {
		Rectangle{0, 14,180,80},
		Rectangle{180, 14,180,80},
		Rectangle{360, 14,180,80},
		Rectangle{540, 14,180,80},
		Rectangle{720, 14,180,80},
		Rectangle{720 + 180, 14,180,80},
		Rectangle{720 + 360, 14,180,80},
		Rectangle{720 + 540, 14,180,80}
	};
	Rectangle buttonDestRect[] = {
		Rectangle{810,10,180,80},
		Rectangle{1010,10,180,80},
		Rectangle{810,110,180,80},
		Rectangle{1010,110,180,80},
		Rectangle{810,210,180,80},
		Rectangle{1010,210,180,80},
		Rectangle{810,310,180,80},
		Rectangle{1010,310,180,80}
	};

	/// TEMP VARS FOR CURRENT BLOCK
	float blockWidth = 100;
	float blockHeight = 100;
	bool blockSides[4] = {true, false, false, false};
	Color blockColor = RandomColor();

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

		// arrow keys resize block
		blockWidth += EDITOR_RESIZE_SPEED * (IsKeyPressed(KEY_RIGHT) - IsKeyPressed(KEY_LEFT));
		if (blockWidth < 25) blockWidth = 25;
		blockHeight += EDITOR_RESIZE_SPEED * (IsKeyPressed(KEY_DOWN) - IsKeyPressed(KEY_UP));
		if (blockHeight < 25) blockHeight = 25;

		if (IsKeyPressed(KEY_I)) blockSides[0] = !blockSides[0];
		if (IsKeyPressed(KEY_L)) blockSides[1] = !blockSides[1];
		if (IsKeyPressed(KEY_K)) blockSides[2] = !blockSides[2];
		if (IsKeyPressed(KEY_J)) blockSides[3] = !blockSides[3];

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

				if (editorFlag == (short)BLOCK)
				{
					// draw block preview
					Vector2 v = GetScreenToWorld2D(GetMousePosition(), camera);
					if (IsKeyDown(KEY_LEFT_CONTROL))
					{
						//v.x = blockWidth*((int)v.x/(int)blockWidth);
						//v.y = blockWidth*((int)v.y/(int)blockWidth);
						v.x = 5*((int)v.x/(int)5);
						v.y = 5*((int)v.y/(int)5);
					}
					env_object obj = env_object(Rectangle{v.x, v.y, blockWidth, blockHeight});
					for(int i = 0; i < 4; i++) obj.sides[i] = blockSides[i];
					obj.color = blockColor;

					DrawEnvObject(obj);

					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && GetMousePosition() < Rectangle{0,0,800,600})
					{
						level.env_objects.push_back(obj);
						blockColor = RandomColor();
					}
				}
				else if (editorFlag == (short)DELETE)
				{
					auto it = level.env_objects.begin();
					Vector2 v = GetScreenToWorld2D(GetMousePosition(), camera);
					auto obj = level.env_objects.end();

					for (; it != level.env_objects.end() && obj == level.env_objects.end(); it++)
					{
						if (v < it->rect)
							obj = it;
					}

					if (obj != level.env_objects.end())
					{
						env_object obj_clone = *obj;
						obj_clone.color = LIGHTGRAY;
						DrawEnvObject(obj_clone);

						if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
						{
							level.env_objects.erase(obj);
						}
					}
				}

			EndMode2D();

            // draw edit panel
            DrawRectangle(800, 0, 400, 600, Color{30,30,30,200});

			// draw buttons
			/// TODO CLEAN UP THIS LOGIC
			for (int i = 0; i < 8; i++)
			{
				buttonValues[i] = ImageButtonSink(buttonDestRect[i], buttonTex, buttonSourceRect[i]);
			}
			if (buttonValues[0])
			{
				editorFlag = (short)BLOCK;
			}
			if (buttonValues[1])
			{
				editorFlag = (short)SIDES;
			}
			if (buttonValues[2])
			{
				editorFlag = (short)DELETE;
			}
			if (buttonValues[3])
			{
				editorFlag = (short)PAINT;
			}

			if (buttonValues[SAVE]) SaveLevelToFile(level,"output.json");

			// draw grid -+ buttons (alternative square brackets)

			// draw block size -+ buttons (alternative arrow keys)

			// jump to coordinates button

			// name field, save and load buttons

            #ifdef DEV_SHOW_MOUSE_POS
			Vector2 v = GetScreenToWorld2D(GetMousePosition(), camera);
			DrawText(FormatText("x=%4.2f\ny=%4.2f", v.x, v.y), GetMouseX(), GetMouseY() - 25, 20, NEARBLACK);
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
