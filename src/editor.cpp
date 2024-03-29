/// TODO
/* CODE CLEANUP
** - CREATE FUNCTION env_level::AddObject(env_object) THAT ASSIGNS UUID'S CORRECTLY
** - ALSO FUNCTION env_level::AddEnvList(env_list) THAT CALLS ::AddObject using iterator
**
** EDITOR FEATURES
** - more insert options including text, start position
** - more details of current zoom level/tile size/perhaps a grid? (probably not)
** - level details section with editable name, goal setting, etc.
**/

#include "raylib.h"
#define NEARBLACK CLITERAL(Color){20, 20, 20, 255}
#define MUSTARD CLITERAL(Color){203, 182, 51, 255}
#define COL_SPIKE CLITERAL(Color){102, 122, 138, 255}

//#define DEV_SHOW_MOUSE_POS

#define CAMERA_SCROLL_SPEED 6.0f
#define EDITOR_RESIZE_SPEED 25.0f
#define EDITOR_MIN_BLOCK_WIDTH 25.0f
#define EDITOR_MIN_BLOCK_HEIGHT 25.0f
#define EDITOR_STAIR_SIZE 10

#include "environment.h"
#include "operators.h"
#include "button.h"

#include "files.h"

// DEV SHIT
#include <iostream>

env_level current_level;
env_level level;

void UpdateLevel(env_level* level);
static Color GetHandleColor(float rPercent, float gPercent, float bPercent);

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

	/** TEXTURES
	=============*/

	Texture2D tex_terrain_bg = LoadTexture("resources/spr/terrain/bg.png");
	RectSet rect_terrain_bg; /**< Mapping from string names to source rects (BACKGROUND). */
	rect_terrain_bg.emplace("flag", Rectangle{0,0,50,50});

	Texture2D tex_terrain_fg = LoadTexture("resources/spr/terrain/fg.png");
	RectSet rect_terrain_fg; /**< Mapping from string names to source rects (FOREGROUND). */

	Texture2D buttonTex = LoadTexture("resources/spr/buttons.png"); /** EDITOR ONLY */
	Texture2D singleButtonTex = LoadTexture("resources/spr/button.png"); /** EDITOR ONLY */
	SetTextureWrap(singleButtonTex, TEXTURE_WRAP_CLAMP);
	// Font font_gamer = LoadFont("resources/gamer.ttf");
	Font font_gamer = LoadFontEx("resources/gamer.ttf", 40, NULL, 94);

	/** SOUNDS
	=============*/


	/** SETUP
	=============*/

	/// COUNTERS AND TIMERS
	int xResizeCounter = 0;
	int yResizeCounter = 0;

	/// BACKGROUND RECTS
	Rectangle editPanel = Rectangle{800, 0, 400, 600}; /**< EDITOR ONLY */
	Rectangle colorPanelRect = Rectangle{520, 140, 265, 140}; /**< EDITOR ONLY */
	Rectangle insertParameterRect = Rectangle{600, 10, 190, 80}; /**< used for insert parameters i.e. boost and launch box strengths TODO IMPLEMENT BOOST VERSION, EDITOR ONLY */
	Rectangle barRect = Rectangle{30,510,740, 70}; /**< EDITOR ONLY */

	/// TEXT FIELD VARS
	Rectangle fileNameRect = Rectangle{810, 410, 380, 80};
	Rectangle fileNameRectSmall = Rectangle{810, 425, 380, 50};
	char fileNameString[31] = {'\0'};
	bool fileNameFocus = false;

	/// INSERT PARAMETER VARS
	float insertParameterBar1 = 0.1f;

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

	/** editor variables */
	bool buttonValues[8] = {false}; /**< Button triggers */
	short editorFlag = -1; /**< Determines the current action by the editor. */
	bool stairMode = false;

	enum INSERT_MODE_SELECTION /**< Enum for following variable */
	{
		INSERT_BLOCK, INSERT_TEXT, INSERT_COIN, INSERT_BOOST, INSERT_LAUNCH, INSERT_HAZARD, INSERT_GOAL, INSERT_KEY, INSERT_DOOR
	};
	INSERT_MODE_SELECTION insertSelection = INSERT_BLOCK; /**< What object is created while in Insert mode. */

	bool insertBar = true; /**< Toggles visibility of insertBar. */


	/** editor buttons */

	enum BUTTON_SOURCE_RECT
	{
		INSERT, DELETE, SIDES, PAINT, COPY, CUT, JUMPTO, JUMPTO_FIELD, TEXTBOX_ONE, TEXTBOX_TWO, TEMP4, SAVE
	};

	Rectangle buttonSourceRect[] =
	{
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

	std::string buttonLabels[] = {
		"INSERT",
		"DELETE",
		"SOLIDS",
		"PAINT",
		"COPY",
		"CUT"
	};

	Rectangle buttonDestRect[] =
	{
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
		Rectangle{1010, 510, 180,80},
	};
	/**=====================================*/

	/// set fps of window
	SetTargetFPS(90);

	/// remove exit key functionality (alt-f4 will always work)
	SetExitKey(-1);

	/** ENVIRONMENT
	=============*/

	bool levelIsLoaded = false;
	std::string file_name = "output.json";
	std::string window_label = "Level Tester";
	// env_level level;

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

		camera.zoom += ((float)GetMouseWheelMove()*0.1f);
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
		camera.target = camera.target + (scrollSpeed * Vector2{((float)IsKeyDown(KEY_D) - (float)IsKeyDown(KEY_A)), ((float)IsKeyDown(KEY_S) - (float)IsKeyDown(KEY_W))});

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
		if (blockWidth < EDITOR_MIN_BLOCK_WIDTH) blockWidth = EDITOR_MIN_BLOCK_WIDTH;

		blockHeight += resizeSpeed * (IsKeyPressed(KEY_DOWN) - IsKeyPressed(KEY_UP));
		if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_UP))
		{
			if (++yResizeCounter > 20 && yResizeCounter % 3 == 0)
				blockHeight += resizeSpeed * (IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP));
		}
		else
			yResizeCounter = 0;
		if (blockHeight < EDITOR_MIN_BLOCK_HEIGHT) blockHeight = EDITOR_MIN_BLOCK_HEIGHT;

		if (IsKeyPressed(KEY_X))
			stairMode = !stairMode;

		if (stairMode)
		{
			blockWidth = EDITOR_STAIR_SIZE;
			blockHeight = EDITOR_STAIR_SIZE;
		}
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

			current_level = LoadLevelFromFile(file_name);
			ClearDroppedFiles();

			// add the atlases (DEV)
			current_level.foregroundAtlas = tex_terrain_fg;
			current_level.foregroundRect = rect_terrain_fg;
			current_level.backgroundAtlas = tex_terrain_bg;
			current_level.backgroundRect = rect_terrain_bg;

			SetWindowTitle(TextFormat("%s %s", "Editing ", GetFileName(file_name.c_str())));
			levelIsLoaded = true;
			fileNameFocus = false;

			strncpy(fileNameString, GetFileNameWithoutExt(file_name.c_str()), 30);
		}

		/// FileNameTextField
		if (SimpleTextBoxUpdate(fileNameRect, fileNameString, 30, &fileNameFocus))
		{
			file_name = TextFormat("%s.json", fileNameString);
			level.label = fileNameString;

			if (!levelIsLoaded)
			{
				levelIsLoaded = true;

				current_level.player_start = Vector2{100,-100};
				current_level.env_objects.push_back(env_object{Rectangle{0,0,200,100}, BLOCK});

				// since the level isn't loaded, create the new file
				SaveLevelToFile(current_level, "levels/" + file_name);
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

		BeginMode2D(camera); /// BEGIN MODE 2D

		//{ LEVEL DRAW
		if (levelIsLoaded)
			DrawLevel(current_level);
		//}

		Vector2 m = GetMousePosition();
		bool __boundsCheck = m < Rectangle{0,0,800,600} && !(m < editPanel);
		if (editorFlag == (short)PAINT)
			__boundsCheck &= !(m < colorPanelRect);
		else if (editorFlag == (short)INSERT)
		{
			__boundsCheck &= !(m < barRect);
			__boundsCheck &= !(m < insertParameterRect);
		}
		if (levelIsLoaded && __boundsCheck)
		{
			if (editorFlag == (short)INSERT)
			{
				// draw block preview
				Vector2 v = GetScreenToWorld2D(GetMousePosition(), camera);
				if (IsKeyDown(KEY_LEFT_CONTROL))
				{
					if (stairMode)
					{
						v.x = EDITOR_STAIR_SIZE*((int)v.x/(int)EDITOR_STAIR_SIZE);
						v.y = EDITOR_STAIR_SIZE*((int)v.y/(int)EDITOR_STAIR_SIZE);
					}
					else
					{
						v.x = EDITOR_MIN_BLOCK_WIDTH*((int)v.x/(int)EDITOR_MIN_BLOCK_WIDTH);
						v.y = EDITOR_MIN_BLOCK_WIDTH*((int)v.y/(int)EDITOR_MIN_BLOCK_HEIGHT);
					}
				}

				env_object obj = env_object(Rectangle{v.x, v.y, blockWidth, blockHeight});

				if (insertSelection == INSERT_BLOCK)
				{
					/// CREATE BLOCK PREVIEW
					obj.setSides(blockSides[0], blockSides[1], blockSides[2], blockSides[3]);
					obj.color = blockColor;
					obj.type = BLOCK;
				}
				else if (insertSelection == INSERT_COIN)
				{
					/// CREATE COIN PREVIEW
					obj.rect = Rectangle{obj.rect.x, obj.rect.y, 100, 100};
					obj.color = Color{245, 224, 66, 200};
					obj.setSides(false, false, false, false);
					obj.type = COIN;
					obj.func = STATIC;
					obj.isCollected = false;
				}
				else if (insertSelection == INSERT_BOOST)
				{
					/// CREATE BOOST PREVIEW
					obj.color = Color{100,100,100,50};
					obj.type = BOOST;
					obj.setSides(blockSides[0],blockSides[1],blockSides[2],blockSides[3]);
				}
				else if (insertSelection == INSERT_LAUNCH)
				{
					/// CREATE LAUNCH PREVIEW
					obj.color = Color{100,250,100,50};
					obj.type = LAUNCH;
					obj.setSides(blockSides[0],blockSides[1],blockSides[2],blockSides[3]);
					obj.boostMag = insertParameterBar1 * 8.0f * LAUNCH_BASE_VALUE;
				}
				else if (insertSelection == INSERT_HAZARD)
				{
					/// CREATE HAZARD PREVIEW
					obj.color = COL_SPIKE;
					obj.type = HAZARD;
					obj.setSides(false, false, false, false);
				}
				else if (insertSelection == INSERT_GOAL)
				{
					/// CREATE HAZARD PREVIEW
					obj.color = WHITE;
					obj.type = GOAL;
					obj.setSides(false, false, false, false);
				}

				/// DRAW PREVIEW
				if (obj.type == TEXT || obj.type == GOAL)
					DrawBGEnvObject(obj, tex_terrain_bg, rect_terrain_bg);
				else
					DrawFGEnvObject(obj, tex_terrain_fg, rect_terrain_fg);

				/// CREATE OBJECT BASED ON PREVIEW
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
				{
					current_level.env_objects.push_back(obj);
					blockColor = Color{(unsigned char)(rPercent * 255), (unsigned char)(gPercent * 255), (unsigned char)(bPercent * 255), (unsigned char)(aPercent * 255)};
				}
			}
			else if (editorFlag == (short)DELETE)
			{
				auto it = current_level.env_objects.end() - 1;
				Vector2 v = GetScreenToWorld2D(GetMousePosition(), camera);
				auto obj = current_level.env_objects.begin() - 1;

				// iterate from the end of the list, since objects near the end of the list appear at the front
				for (; it != current_level.env_objects.begin() - 1 && obj == current_level.env_objects.begin() - 1; it--)
				{
					if (v < it->rect)
						obj = it;
				}

				if (obj != current_level.env_objects.begin() - 1)
				{
					env_object obj_clone = *obj;
					obj_clone.color = Color{255, 211, 209, 255};
					DrawEnvObject(obj_clone);

					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
					{
						current_level.env_objects.erase(obj);
					}
				}
			}
			else if (editorFlag == (short)PAINT)
			{
				auto it = current_level.env_objects.end() - 1;
				Vector2 v = GetScreenToWorld2D(GetMousePosition(), camera);
				auto obj = current_level.env_objects.begin() - 1;

				// iterate from the end of the list, since objects near the end of the list appear at the front
				for (; it != current_level.env_objects.begin() - 1 && obj == current_level.env_objects.begin() - 1; it--)
				{
					if (v < it->rect)
						obj = it;
				}

				if (obj != current_level.env_objects.begin() - 1)
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
				auto it = current_level.env_objects.end() - 1;
				Vector2 v = GetScreenToWorld2D(GetMousePosition(), camera);
				auto obj = current_level.env_objects.begin() - 1;

				// iterate from the end of the list, since objects near the end of the list appear at the front
				for (; it != current_level.env_objects.begin() - 1 && obj == current_level.env_objects.begin() - 1; it--)
				{
					if (v < it->rect)
						obj = it;
				}

				if (obj != current_level.env_objects.begin() - 1)
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
				auto it = current_level.env_objects.end() - 1;
				Vector2 v = GetScreenToWorld2D(GetMousePosition(), camera);
				auto obj = current_level.env_objects.begin() - 1;

				// iterate from the end of the list, since objects near the end of the list appear at the front
				for (; it != current_level.env_objects.begin() - 1 && obj == current_level.env_objects.begin() - 1; it--)
				{
					if (v < it->rect)
						obj = it;
				}

				if (obj != current_level.env_objects.begin() - 1)
				{
					env_object obj_clone = *obj;
					obj_clone.color = Color{233, 255, 186, 255};
					DrawEnvObject(obj_clone);

					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
					{
						blockWidth = obj->rect.width;
						blockHeight = obj->rect.height;
						blockColor = obj->color;
						rPercent = (float)blockColor.r / 255.0f;
						gPercent = (float)blockColor.g / 255.0f;
						bPercent = (float)blockColor.b / 255.0f;
						aPercent = (float)blockColor.a / 255.0f;

						blockSides[0] = obj->sides[0];
						blockSides[1] = obj->sides[1];
						blockSides[2] = obj->sides[2];
						blockSides[3] = obj->sides[3];

						editorFlag = (short)INSERT;
						if (obj->type == BLOCK)
							insertSelection = INSERT_BLOCK;
						else if (obj->type == COIN)
							insertSelection = INSERT_COIN;
						else if (obj->type == BOOST)
							insertSelection = INSERT_BOOST;
						else	// select block by default
							insertSelection = INSERT_BLOCK;
					}
				}
			}
			else if (editorFlag == (short)CUT)
			{
				auto it = current_level.env_objects.end() - 1;
				Vector2 v = GetScreenToWorld2D(GetMousePosition(), camera);
				auto obj = current_level.env_objects.begin() - 1;

				// iterate from the end of the list, since objects near the end of the list appear at the front
				for (; it != current_level.env_objects.begin() - 1 && obj == current_level.env_objects.begin() - 1; it--)
				{
					if (v < it->rect)
						obj = it;
				}

				if (obj != current_level.env_objects.begin() - 1)
				{
					env_object obj_clone = *obj;
					obj_clone.color = Color{255, 242, 209, 255};
					DrawEnvObject(obj_clone);

					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
					{
						blockWidth = obj->rect.width;
						blockHeight = obj->rect.height;
						blockColor = obj->color;
						rPercent = (float)blockColor.r / 255.0f;
						gPercent = (float)blockColor.g / 255.0f;
						bPercent = (float)blockColor.b / 255.0f;
						aPercent = (float)blockColor.a / 255.0f;
						blockSides[0] = obj->sides[0];
						blockSides[1] = obj->sides[1];
						blockSides[2] = obj->sides[2];
						blockSides[3] = obj->sides[3];

						current_level.env_objects.erase(obj);

						editorFlag = (short)INSERT;
						if (obj->type == BLOCK)
							insertSelection = INSERT_BLOCK;
						else if (obj->type == COIN)
							insertSelection = INSERT_COIN;
						else if (obj->type == BOOST)
							insertSelection = INSERT_BOOST;
						else	// select block by default
							insertSelection = INSERT_BLOCK;
					}
				}
			}
		}

		EndMode2D(); /// END MODE 2D

		// if level is not loaded put some instructions
		if(!levelIsLoaded)
		{
			//DrawTextRec(GetFontDefault(), "DRAG AND DROP A LEVEL TO EDIT OR TYPE A NAME AND PRESS ENTER", Rectangle{200,200,400,400}, 40, 1.0f, true, LIGHTGRAY);
			DrawText("DRAG AND DROP A LEVEL TO EDIT\nOR TYPE A NAME AND PRESS ENTER", 50,200, 30, LIGHTGRAY);
		}

		/// DRAW INSERT BAR (ON BOTTOM)
		if (editorFlag == (short)INSERT && insertBar)
		{
			DrawRectangleRec(barRect, Color{30,30,30,200});

			/// draw block icon
			Rectangle blockR = Rectangle{barRect.x + 10, barRect.y + 10, 50, 50};
			env_object block;
			block.rect = blockR;
			block.type = BLOCK;
			block.color = blockColor;
			block.setSides(blockSides[0], blockSides[1], blockSides[2], blockSides[3]);
			DrawFGEnvObject(block, tex_terrain_fg, rect_terrain_fg);
			if(HiddenButton(blockR))
			{
				// block insert
				insertSelection = INSERT_BLOCK;
			}

			/// draw coin icon
			Rectangle _blockR = Rectangle{barRect.x + 10 + 60, barRect.y + 10, 50, 50};
			env_object _block;
			_block.rect = _blockR;
			_block.color = Color{245, 224, 66, 200};
			_block.type = COIN;
			DrawFGEnvObject(_block, tex_terrain_fg, rect_terrain_fg);
			if (HiddenButton(_blockR))
			{
				// coin insert
				insertSelection = INSERT_COIN;
			}

			/// draw boost icon
			_blockR = Rectangle{barRect.x + 10 + 120, barRect.y + 10, 50, 50};
			_block.setSides(blockSides[0], blockSides[1], blockSides[2], blockSides[3]);
			_block.rect = _blockR;
			_block.color = Color{100, 100, 100, 200};
			_block.type = BOOST;

			DrawFGEnvObject(_block, tex_terrain_fg, rect_terrain_fg);

			if (HiddenButton(_blockR))
			{
				// coin insert
				insertSelection = INSERT_BOOST;
			}

			/// draw launch icon
			_blockR = Rectangle{barRect.x + 10 + 180, barRect.y + 10, 50, 50};
			_block.setSides(blockSides[0], blockSides[1], blockSides[2], blockSides[3]);
			_block.rect = _blockR;
			_block.color = Color{100, 255, 100, 200};
			_block.type = LAUNCH;

			DrawFGEnvObject(_block, tex_terrain_fg, rect_terrain_fg);

			if (HiddenButton(_blockR))
			{
				// coin insert
				insertSelection = INSERT_LAUNCH;
			}

			/// draw hazard icon
			_blockR.x = barRect.x + 10 + 240;
			_block.setSides(false,false,false,false);
			_block.rect = _blockR;
			_block.color = COL_SPIKE;
			_block.type = HAZARD;
			DrawFGEnvObject(_block, tex_terrain_fg, rect_terrain_fg);
			if (HiddenButton(_blockR))
			{
				// hazard insert
				insertSelection = INSERT_HAZARD;
			}

			/// draw goal icon
			_blockR.x = barRect.x + 10 + 300;
			_block.setSides(false,false,false,false);
			_block.rect = _blockR;
			_block.color = WHITE;
			_block.type = GOAL;
			DrawBGEnvObject(_block, tex_terrain_bg, rect_terrain_bg);
			if (HiddenButton(_blockR))
			{
				// goal insert
				insertSelection = INSERT_GOAL;
			}

			/// draw key icon
		}

		// draw edit panel
		DrawRectangleRec(editPanel, Color{30,30,30,200});

		// draw buttons
		for (int i = 0; i < 6; i++)
		{
			SetTextButtonOffset(0, -7);
			buttonValues[i] = ImageTextButtonSink(buttonDestRect[i], singleButtonTex, Rectangle{0,0,180,80}, buttonLabels[i], font_gamer);
			UnsetTextButtonOffset();
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

		if (editorFlag == (short)INSERT && insertSelection == INSERT_LAUNCH)
		{
			DrawRectangleRec(insertParameterRect, Color{0,0,0,40});
			Vector2 start{insertParameterRect.x + 20, insertParameterRect.y + 30};
			SliderBar(start, 150, &insertParameterBar1);

			DrawText("LAUNCH PAD STR.", (int)insertParameterRect.x + 20, (int)insertParameterRect.y + 55, 15, BLACK);
		}

		// draw the color panel
		if (editorFlag == (short)PAINT)
		{
			/// DRAW COLOR PANEL
			DrawRectangleRec(colorPanelRect, Color{0,0,0,40});

			blockColor = Color{(unsigned char)(rPercent * 255), (unsigned char)(gPercent * 255), (unsigned char)(bPercent * 255), (unsigned char)(aPercent * 255)};

			DrawRectangle(colorPanelRect.x + 10, colorPanelRect.y + 20, 40, 100, blockColor);
			Vector2 __v;

			__v = Vector2{colorPanelRect.x + 80,colorPanelRect.y + 20};
			SliderBar(__v, 100, &rPercent, GetHandleColor(rPercent, 0, 0),1.0f, false);

			__v = Vector2{colorPanelRect.x + 130,colorPanelRect.y + 20};
			SliderBar(__v, 100, &gPercent, GetHandleColor(0, gPercent, 0),1.0f, false);

			__v = Vector2{colorPanelRect.x + 180,colorPanelRect.y + 20};
			SliderBar(__v, 100, &bPercent, GetHandleColor(0, 0, bPercent),1.0f, false);

			__v = Vector2{colorPanelRect.x + 230,colorPanelRect.y + 20};
			unsigned char rgb = (unsigned char)(255 - 255*aPercent);
			SliderBar(__v, 100, &aPercent, Color{rgb, rgb, rgb, 255},1.0f, false);
			/// END COLOR PANEL
		}

		// draw grid -+ buttons (alternative square brackets)

		// draw block size -+ buttons (alternative arrow keys)

		/// SAVE BUTTONS
		if (ImageButtonSink(buttonDestRect[SAVE], buttonTex, buttonSourceRect[SAVE]))
		{
			file_name = TextFormat("%s.json", fileNameString);
			current_level.label = fileNameString;

			if (!levelIsLoaded)
			{
				levelIsLoaded = true;

				current_level.player_start = Vector2{100,-100};
				current_level.env_objects.push_back(env_object{Rectangle{0,0,200,100}, BLOCK});
			}

			SaveLevelToFile(current_level, "levels/" + file_name);
		}

		SimpleTextBoxDraw(fileNameRectSmall, fileNameString, 30, &fileNameFocus);
		/// END SAVE BUTTONS

#ifdef DEV_SHOW_MOUSE_POS
		Vector2 v = GetScreenToWorld2D(GetMousePosition(), camera);
		//DrawText(TextFormat("x=%4.2f\ny=%4.2f", v.x, v.y), GetMouseX(), GetMouseY() - 25, 20, NEARBLACK);
		DrawText(TextFormat("x=%3d\ny=%3d", GetMouseX(), GetMouseY()), GetMouseX(), GetMouseY() - 25, 20, NEARBLACK);
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

// Color handleCol = Color{255, (255 - 255*rPercent), (255 - 255*rPercent), 255};
// handleCol = Color{(255 - 255*gPercent), 255, (255 - 255*gPercent), 255};
// handleCol = Color{(255 - 255*bPercent), (255 - 255*bPercent), 255, 255};
static Color GetHandleColor(float rPercent, float gPercent, float bPercent)
{
	return Color{(unsigned char)(255 - 255*rPercent), (unsigned char)(255 - 255*gPercent), (unsigned char)(255 - 255*bPercent), 255};
}