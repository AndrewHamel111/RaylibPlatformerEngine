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
    std::string file_name = "output.json";
    std::string window_label = "Level Tester";
    env_level level;

    /// COUNTERS AND TIMERS
    int xResizeCounter = 0;
    int yResizeCounter = 0;

    char xJumpToStr[6] = {'\0'};
    char yJumpToStr[6] = {'\0'};
    bool xJumpToFocus = false;
    bool yJumpToFocus = false;

    /// LOAD BUTTONS AND ASSIGN SOURCE/DEST RECTS, EDITOR FLAGS
	Texture2D buttonTex = LoadTexture("sprites/buttons.png");
	bool buttonValues[8] = {false};
	short editorFlag = -1;
	enum BUTTON_SOURCE_RECT {
		INSERT, DELETE, SIDES, PAINT, COPY, CUT, JUMPTO, JUMPTO_FIELD, TEXTBOX_ONE, TEXTBOX_TWO, TEMP4, SAVE
	};
	Rectangle buttonSourceRect[] = {
		Rectangle{0, 14,180,80},	// INSERT
		Rectangle{180, 14,180,80},	// DELETE
		Rectangle{360, 14,180,80},	// SIDES
		Rectangle{540, 14,180,80},	// PAINT
		Rectangle{720, 14,180,80}, // COPY
		Rectangle{720 + 180, 14,180,80}, // CUT
		Rectangle{720 + 360, 14,180,80}, // JUMPTO
		Rectangle{0, 14,180,80}, // TEMP3
		Rectangle{0, 14,180,80}, // TEXTBOX SPACER
		Rectangle{0, 14,180,80}, // TEXTBOX SPACER
		Rectangle{0, 14,180,80},	// TEMP4
		Rectangle{720 + 540, 14,180,80} // SAVE
	};
	Rectangle buttonDestRect[] = {
		Rectangle{810,10,180,80},
		Rectangle{1010,10,180,80},
		Rectangle{810,110,180,80},
		Rectangle{1010,110,180,80},
		Rectangle{810,210,180,80},
		Rectangle{1010,210,180,80},
		Rectangle{1010,310,180,80},
		Rectangle{810,310,80,80},
		Rectangle{910,310,80,80},
		Rectangle{0,0,0,0},
		Rectangle{0,0,0,0},
		Rectangle{1010, 510, 180 ,80},
	};

	/// TEXT FIELD VARS
	Rectangle fileNameRect = Rectangle{810, 410, 380, 80};
	Rectangle fileNameRectSmall = Rectangle{810, 425, 380, 50};
	char fileNameString[31] = {'\0'};
	bool fileNameFocus = false;

	/// COLOR PICKER VARS
	float rPercent = 0.5f;
	float gPercent = 0.5f;
	float bPercent = 0.5f;
	float aPercent = 1.0f;

	/// TEMP VARS FOR CURRENT BLOCK
	float blockWidth = 100;
	float blockHeight = 100;
	bool blockSides[4] = {true, false, false, false};
	Color blockColor = Color{(unsigned char)(rPercent * 255), (unsigned char)(gPercent * 255), (unsigned char)(bPercent * 255), (unsigned char)(aPercent * 255)};

	/// set fps of window
    SetTargetFPS(90);

	/// remove exit key functionality (alt-f4 will always work)
	SetExitKey(-1);

	/// Create the camera object
	Camera2D camera{Vector2{0,0}, Vector2{0, 0}, 0, 0.5f};
	camera.offset = {550, 300};

    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update variables / Implement example logic at this point
        //----------------------------------------------------------------------------------

        camera.zoom += ((float)GetMouseWheelMove()*0.025f);
        //camera.rotation += 0.5f;

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.25f) camera.zoom = 0.25f;

        if (IsKeyPressed(KEY_R))
        {
            camera.zoom = 1.0f;
        }

        float scrollSpeed = CAMERA_SCROLL_SPEED;
        if (IsKeyDown(KEY_LEFT_SHIFT)) scrollSpeed *= 3;

		/// WASD scrolls camera
		camera.target = camera.target + (scrollSpeed * Vector2{(IsKeyDown(KEY_D) - IsKeyDown(KEY_A)), (IsKeyDown(KEY_S) - IsKeyDown(KEY_W))});

		int resizeSpeed = (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) ? 100 : EDITOR_RESIZE_SPEED;

		/// RESIZE BLOCK
		blockWidth += resizeSpeed * (IsKeyPressed(KEY_RIGHT) - IsKeyPressed(KEY_LEFT));
		if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_LEFT))
		{
			if (++xResizeCounter > 20 && xResizeCounter % 3 == 0)
				blockWidth += resizeSpeed * (IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT));
		}
		else
			xResizeCounter = 0;
		if (blockWidth < 25) blockWidth = 25;

		blockHeight += resizeSpeed * (IsKeyPressed(KEY_DOWN) - IsKeyPressed(KEY_UP));
		if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_UP))
		{
			if (++yResizeCounter > 20 && yResizeCounter % 3 == 0)
				blockHeight += resizeSpeed * (IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP));
		}
		else
			yResizeCounter = 0;
		if (blockHeight < 25) blockHeight = 25;
		/// END RESIZE BLOCK

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

			strncpy(fileNameString, GetFileNameWithoutExt(file_name.c_str()), 30);
		}

		/// FileNameTextField
		if (SimpleTextBoxUpdate(fileNameRect, fileNameString, 30, &fileNameFocus))
		{
			file_name = FormatText("%s.json", fileNameString);
			level.label = fileNameString;

			if (!levelIsLoaded)
			{
				levelIsLoaded = true;

				level.player_start = Vector2{100,-100};
				level.env_objects.push_back(env_object{Rectangle{0,0,200,100}, BLOCK});

				// since the level isn't loaded, create the new file
				SaveLevelToFile(level, "levels/" + file_name);
			}
		}

		/*
		/// JUMP TO field updates
		NumberBoxUpdate(buttonDestRect[JUMPTO_FIELD], xJumpToStr, 5, &xJumpToFocus);
		Rectangle _r = buttonDestRect[JUMPTO_FIELD];
		_r.x += 100;
		NumberBoxUpdate(_r, yJumpToStr, 5, &yJumpToFocus);
		/// END JUMP TO fields
		*/

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(WHITE);

            BeginMode2D(camera);

                //{ LEVEL DRAW
                if (levelIsLoaded)
					DrawLevel(level);
                //}

                if (levelIsLoaded && GetMousePosition() < Rectangle{0,0,800,600})
				{
					if (editorFlag == (short)INSERT)
					{
						// draw block preview
						Vector2 v = GetScreenToWorld2D(GetMousePosition(), camera);
						if (IsKeyDown(KEY_LEFT_CONTROL))
						{
							v.x = 25*((int)v.x/(int)25);
							v.y = 25*((int)v.y/(int)25);
						}
						env_object obj = env_object(Rectangle{v.x, v.y, blockWidth, blockHeight});
						obj.setSides(blockSides[0], blockSides[1], blockSides[2], blockSides[3]);
						obj.color = blockColor;

						DrawEnvObject(obj);

						if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
						{
							level.env_objects.push_back(obj);
							blockColor = Color{(unsigned char)(rPercent * 255), (unsigned char)(gPercent * 255), (unsigned char)(bPercent * 255), (unsigned char)(aPercent * 255)};
						}
					}
					else if (editorFlag == (short)DELETE)
					{
						auto it = level.env_objects.end() - 1;
						Vector2 v = GetScreenToWorld2D(GetMousePosition(), camera);
						auto obj = level.env_objects.begin() - 1;

						// iterate from the end of the list, since objects near the end of the list appear at the front
						for (; it != level.env_objects.begin() - 1 && obj == level.env_objects.begin() - 1; it--)
						{
							if (v < it->rect)
								obj = it;
						}

						if (obj != level.env_objects.begin() - 1)
						{
							env_object obj_clone = *obj;
							obj_clone.color = Color{255, 211, 209, 255};
							DrawEnvObject(obj_clone);

							if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
							{
								level.env_objects.erase(obj);
							}
						}
					}
					else if (editorFlag == (short)PAINT)
					{
						auto it = level.env_objects.end() - 1;
						Vector2 v = GetScreenToWorld2D(GetMousePosition(), camera);
						auto obj = level.env_objects.begin() - 1;

						// iterate from the end of the list, since objects near the end of the list appear at the front
						for (; it != level.env_objects.begin() - 1 && obj == level.env_objects.begin() - 1; it--)
						{
							if (v < it->rect)
								obj = it;
						}

						if (obj != level.env_objects.begin() - 1)
						{
							env_object obj_clone = *obj;
							obj_clone.color = blockColor;
							DrawEnvObject(obj_clone);

							if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
							{
								obj->color = blockColor;
							}
							else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
							{
								blockColor = obj->color;
								rPercent = (float)blockColor.r / 255.0f;
								gPercent = (float)blockColor.g / 255.0f;
								bPercent = (float)blockColor.b / 255.0f;
								aPercent = (float)blockColor.a / 255.0f;
							}
						}
					}
					else if (editorFlag == (short)SIDES)
					{
						auto it = level.env_objects.end() - 1;
						Vector2 v = GetScreenToWorld2D(GetMousePosition(), camera);
						auto obj = level.env_objects.begin() - 1;

						// iterate from the end of the list, since objects near the end of the list appear at the front
						for (; it != level.env_objects.begin() - 1 && obj == level.env_objects.begin() - 1; it--)
						{
							if (v < it->rect)
								obj = it;
						}

						if (obj != level.env_objects.begin() - 1)
						{
							env_object obj_clone = *obj;
							obj_clone.color = LIGHTGRAY;
							obj_clone.setSides(blockSides[0], blockSides[1], blockSides[2], blockSides[3]);
							DrawEnvObject(obj_clone);

							if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
							{
								obj->setSides(blockSides[0], blockSides[1], blockSides[2], blockSides[3]);
							}
						}
					}
					else if (editorFlag == (short)COPY)
					{
						auto it = level.env_objects.end() - 1;
						Vector2 v = GetScreenToWorld2D(GetMousePosition(), camera);
						auto obj = level.env_objects.begin() - 1;

						// iterate from the end of the list, since objects near the end of the list appear at the front
						for (; it != level.env_objects.begin() - 1 && obj == level.env_objects.begin() - 1; it--)
						{
							if (v < it->rect)
								obj = it;
						}

						if (obj != level.env_objects.begin() - 1)
						{
							env_object obj_clone = *obj;
							obj_clone.color = Color{233, 255, 186, 255};
							DrawEnvObject(obj_clone);

							if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
							{
								blockWidth = obj->rect.width;
								blockHeight = obj->rect.height;
								blockColor = obj->color;
								blockSides[0] = obj->sides[0];
								blockSides[1] = obj->sides[1];
								blockSides[2] = obj->sides[2];
								blockSides[3] = obj->sides[3];

								editorFlag = (short)INSERT;
							}
						}
					}
					else if (editorFlag == (short)CUT)
					{
						auto it = level.env_objects.end() - 1;
						Vector2 v = GetScreenToWorld2D(GetMousePosition(), camera);
						auto obj = level.env_objects.begin() - 1;

						// iterate from the end of the list, since objects near the end of the list appear at the front
						for (; it != level.env_objects.begin() - 1 && obj == level.env_objects.begin() - 1; it--)
						{
							if (v < it->rect)
								obj = it;
						}

						if (obj != level.env_objects.begin() - 1)
						{
							env_object obj_clone = *obj;
							obj_clone.color = Color{255, 242, 209, 255};
							DrawEnvObject(obj_clone);

							if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
							{
								blockWidth = obj->rect.width;
								blockHeight = obj->rect.height;
								blockColor = obj->color;
								blockSides[0] = obj->sides[0];
								blockSides[1] = obj->sides[1];
								blockSides[2] = obj->sides[2];
								blockSides[3] = obj->sides[3];

								level.env_objects.erase(obj);

								editorFlag = (short)INSERT;
							}
						}
					}
				}

			EndMode2D();

			// if level is not loaded put some instructions
			if(!levelIsLoaded)
			{
				DrawTextRec(GetFontDefault(), "DRAG AND DROP A LEVEL TO EDIT OR TYPE A NAME AND PRESS ENTER", Rectangle{200,200,400,400}, 40, 1.0f, true, LIGHTGRAY);
			}

            // draw edit panel
            DrawRectangle(800, 0, 400, 600, Color{30,30,30,200});

			// draw buttons
			/// TODO CLEAN UP THIS LOGIC
			for (int i = 0; i < 6; i++)
			{
				buttonValues[i] = ImageButtonSink(buttonDestRect[i], buttonTex, buttonSourceRect[i]);
			}
			if (buttonValues[0])
			{
				editorFlag = (short)INSERT;
			}
			if (buttonValues[1])
			{
				editorFlag = (short)DELETE;
			}
			if (buttonValues[2])
			{
				editorFlag = (short)SIDES;
			}
			if (buttonValues[3])
			{
				editorFlag = (short)PAINT;
			}
			if (buttonValues[4])
			{
				editorFlag = (short)COPY;
			}
			if (buttonValues[5])
			{
				editorFlag = (short)CUT;
			}

			// draw the color panel
			if (editorFlag == (short)PAINT)
			{
				/// DRAW COLOR PANEL
				Rectangle colorPanelRect = Rectangle{520, 140, 265, 140};
				DrawRectangleRec(colorPanelRect, Color{0,0,0,40});

				blockColor = Color{(unsigned char)(rPercent * 255), (unsigned char)(gPercent * 255), (unsigned char)(bPercent * 255), (unsigned char)(aPercent * 255)};

				DrawRectangle(colorPanelRect.x + 10, colorPanelRect.y + 20, 40, 100, blockColor);

				Vector2 __v = Vector2{colorPanelRect.x + 80,colorPanelRect.y + 20};
				Color handleCol = Color{255, (255 - 255*rPercent), (255 - 255*rPercent), 255};
				SliderBar(__v, 100, &rPercent, handleCol,1.0f, false);

				__v = Vector2{colorPanelRect.x + 130,colorPanelRect.y + 20};
				handleCol = Color{(255 - 255*gPercent), 255, (255 - 255*gPercent), 255};
				SliderBar(__v, 100, &gPercent, handleCol,1.0f, false);

				__v = Vector2{colorPanelRect.x + 180,colorPanelRect.y + 20};
				handleCol = Color{(255 - 255*bPercent), (255 - 255*bPercent), 255, 255};
				SliderBar(__v, 100, &bPercent, handleCol,1.0f, false);

				__v = Vector2{colorPanelRect.x + 230,colorPanelRect.y + 20};
				handleCol = Color{(255 - 255*aPercent), (255 - 255*aPercent), (255 - 255*aPercent), 255};
				SliderBar(__v, 100, &aPercent, handleCol,1.0f, false);
				/// END COLOR PANEL
			}

			// draw grid -+ buttons (alternative square brackets)

			// draw block size -+ buttons (alternative arrow keys)

			/*
			/// JUMP TO
			if (ImageButtonSink(buttonDestRect[JUMPTO], buttonTex, buttonSourceRect[JUMPTO]))
			{
				camera.target = Vector2{atoi(xJumpToStr), atoi(yJumpToStr)};
			}

			SimpleTextBoxDraw(buttonDestRect[JUMPTO_FIELD], xJumpToStr, 5, &xJumpToFocus);
			Rectangle __r = buttonDestRect[JUMPTO_FIELD];
			__r.x += 100;
			SimpleTextBoxDraw(__r, yJumpToStr, 5, &yJumpToFocus);
			/// END JUMP TO
			*/

			/// SAVE BUTTONS
			if (ImageButtonSink(buttonDestRect[SAVE], buttonTex, buttonSourceRect[SAVE]))
			{
				file_name = FormatText("%s.json", fileNameString);
				level.label = fileNameString;

				if (!levelIsLoaded)
				{
					levelIsLoaded = true;

					level.player_start = Vector2{100,-100};
					level.env_objects.push_back(env_object{Rectangle{0,0,200,100}, BLOCK});
				}

				SaveLevelToFile(level, "levels/" + file_name);
			}

			SimpleTextBoxDraw(fileNameRectSmall, fileNameString, 30, &fileNameFocus);
			/// END SAVE BUTTONS

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
